/* Copyright (c) 2007 Scott Lembcke
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#import <stdlib.h>
#import <stdio.h>
#import <string.h>
#import <math.h>

#import <Chipmunk/Chipmunk.h>

cpTimestamp cp_contact_persistence = 3;

#pragma mark Contact Set Helpers

// Equal function for contactSet.
static cpBool
contactSetEql(cpShape **shapes, cpArbiter *arb)
{
	cpShape *a = shapes[0];
	cpShape *b = shapes[1];
	
	return ((a == arb->private_a && b == arb->private_b) || (b == arb->private_a && a == arb->private_b));
}

// Transformation function for contactSet.
static void *
contactSetTrans(cpShape **shapes, cpSpace *space)
{
	if(space->pooledArbiters->num == 0){
		// arbiter pool is exhausted, make more
		int count = CP_BUFFER_BYTES/sizeof(cpArbiter);
		cpAssert(count, "Buffer size too small.");
		
		cpArbiter *buffer = (cpArbiter *)cpmalloc(CP_BUFFER_BYTES);
		cpArrayPush(space->allocatedBuffers, buffer);
		
		for(int i=0; i<count; i++) cpArrayPush(space->pooledArbiters, buffer + i);
	}
	
	return cpArbiterInit((cpArbiter *) cpArrayPop(space->pooledArbiters), shapes[0], shapes[1]);
}

#pragma mark Collision Pair Function Helpers

// Equals function for collFuncSet.
static cpBool
collFuncSetEql(cpCollisionHandler *check, cpCollisionHandler *pair)
{
	return ((check->a == pair->a && check->b == pair->b) || (check->b == pair->a && check->a == pair->b));
}

// Transformation function for collFuncSet.
static void *
collFuncSetTrans(cpCollisionHandler *handler, void *unused)
{
	cpCollisionHandler *copy = (cpCollisionHandler *)cpmalloc(sizeof(cpCollisionHandler));
	(*copy) = (*handler);
	
	return copy;
}

#pragma mark Post Step Function Helpers

typedef struct postStepCallback {
	cpPostStepFunc func;
	void *obj;
	void *data;
} postStepCallback;

static cpBool
postStepFuncSetEql(postStepCallback *a, postStepCallback *b){
	return a->obj == b->obj;
}

static void *
postStepFuncSetTrans(postStepCallback *callback, void *ignored)
{
	postStepCallback *value = (postStepCallback *)cpmalloc(sizeof(postStepCallback));
	(*value) = (*callback);
	
	return value;
}

#pragma mark Misc Helper Funcs

// Default collision functions.
static cpBool alwaysCollide(cpArbiter *arb, cpSpace *space, void *data){return 1;}
static void nothing(cpArbiter *arb, cpSpace *space, void *data){}

// BBfunc callback for the spatial hash.
static cpBB shapeBBFunc(cpShape *shape){return shape->bb;}

// Iterator functions for destructors.
static void             freeWrap(void         *ptr, void *unused){            cpfree(ptr);}
static void        shapeFreeWrap(cpShape      *ptr, void *unused){     cpShapeFree(ptr);}
static void         bodyFreeWrap(cpBody       *ptr, void *unused){      cpBodyFree(ptr);}
static void   constraintFreeWrap(cpConstraint *ptr, void *unused){cpConstraintFree(ptr);}

#pragma mark Memory Management Functions

#define CP_CONTACTS_BUFFER_SIZE ((CP_BUFFER_BYTES - sizeof(cpContactBufferHeader))/sizeof(cpContact))
typedef struct cpContactBuffer {
	cpContactBufferHeader header;
	cpContact contacts[CP_CONTACTS_BUFFER_SIZE];
} cpContactBuffer;

static cpContactBufferHeader *
cpSpaceAllocContactBuffer(cpSpace *space)
{
	cpContactBuffer *buffer = (cpContactBuffer *)malloc(sizeof(cpContactBuffer));
	cpArrayPush(space->allocatedBuffers, buffer);
	return (cpContactBufferHeader *)buffer;
}

static cpContactBufferHeader *
cpContactBufferHeaderInit(cpContactBufferHeader *header, cpSpace *space)
{
	header->stamp = space->stamp;
	header->next = space->contactBuffersTail;
	header->numContacts = 0;
	
	return header;
}

cpSpace *
cpSpaceAlloc(void)
{
	return (cpSpace *)cpcalloc(1, sizeof(cpSpace));
}

#define DEFAULT_DIM_SIZE 100.0f
#define DEFAULT_COUNT 1000
#define DEFAULT_ITERATIONS 10
#define DEFAULT_ELASTIC_ITERATIONS 0

cpCollisionHandler defaultHandler = {0, 0, alwaysCollide, alwaysCollide, nothing, nothing, NULL};

cpSpace*
cpSpaceInit(cpSpace *space)
{
	space->iterations = DEFAULT_ITERATIONS;
	space->elasticIterations = DEFAULT_ELASTIC_ITERATIONS;
//	space->sleepTicks = 300;
	
	space->gravity = cpvzero;
	space->damping = 1.0f;
	
	space->locked = 0;
	space->stamp = 0;

	space->staticShapes = cpSpaceHashNew(DEFAULT_DIM_SIZE, DEFAULT_COUNT, (cpSpaceHashBBFunc)shapeBBFunc);
	space->activeShapes = cpSpaceHashNew(DEFAULT_DIM_SIZE, DEFAULT_COUNT, (cpSpaceHashBBFunc)shapeBBFunc);
	
	space->allocatedBuffers = cpArrayNew(0);
	
	space->bodies = cpArrayNew(0);
	space->sleepingComponents = cpArrayNew(0);
	space->sleepTimeThreshold = INFINITY;
	space->idleSpeedThreshold = 0.0f;
	
	space->arbiters = cpArrayNew(0);
	space->pooledArbiters = cpArrayNew(0);
	
	cpContactBufferHeader *header = cpContactBufferHeaderInit(cpSpaceAllocContactBuffer(space), space);
	space->contactBuffersHead = header;
	space->contactBuffersTail = header;
	header->next = header; // Buffers will form a ring, start the ring explicitly
	
	space->contactSet = cpHashSetNew(0, (cpHashSetEqlFunc)contactSetEql, (cpHashSetTransFunc)contactSetTrans);
	
	space->constraints = cpArrayNew(0);
	
	space->defaultHandler = defaultHandler;
	space->collFuncSet = cpHashSetNew(0, (cpHashSetEqlFunc)collFuncSetEql, (cpHashSetTransFunc)collFuncSetTrans);
	space->collFuncSet->default_value = &space->defaultHandler;
	
	space->postStepCallbacks = cpHashSetNew(0, (cpHashSetEqlFunc)postStepFuncSetEql, (cpHashSetTransFunc)postStepFuncSetTrans);
	
	cpBodyInit(&space->staticBody, INFINITY, INFINITY);
	space->staticBody.space = space;
	
	return space;
}

cpSpace*
cpSpaceNew(void)
{
	return cpSpaceInit(cpSpaceAlloc());
}

void
cpSpaceDestroy(cpSpace *space)
{
	cpSpaceHashFree(space->staticShapes);
	cpSpaceHashFree(space->activeShapes);
	
	cpArrayFree(space->bodies);
	cpArrayFree(space->sleepingComponents);
	
	cpArrayFree(space->constraints);
	
	cpHashSetFree(space->contactSet);
	
	cpArrayFree(space->arbiters);
	cpArrayFree(space->pooledArbiters);
	
	if(space->allocatedBuffers){
		cpArrayEach(space->allocatedBuffers, freeWrap, NULL);
		cpArrayFree(space->allocatedBuffers);
	}
	
	if(space->postStepCallbacks){
		cpHashSetEach(space->postStepCallbacks, freeWrap, NULL);
		cpHashSetFree(space->postStepCallbacks);
	}
	
	if(space->collFuncSet){
		cpHashSetEach(space->collFuncSet, freeWrap, NULL);
		cpHashSetFree(space->collFuncSet);
	}
}

void
cpSpaceFree(cpSpace *space)
{
	if(space){
		cpSpaceDestroy(space);
		cpfree(space);
	}
}

void
cpSpaceFreeChildren(cpSpace *space)
{
	cpArray *components = space->sleepingComponents;
	while(components->num) cpBodyActivate((cpBody *)components->arr[0]);
	
	cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)&shapeFreeWrap, NULL);
	cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)&shapeFreeWrap, NULL);
	cpArrayEach(space->bodies,           (cpArrayIter)&bodyFreeWrap,          NULL);
	cpArrayEach(space->constraints,      (cpArrayIter)&constraintFreeWrap,    NULL);
}

#pragma mark Collision Handler Function Management

void
cpSpaceAddCollisionHandler(
	cpSpace *space,
	cpCollisionType a, cpCollisionType b,
	cpCollisionBeginFunc begin,
	cpCollisionPreSolveFunc preSolve,
	cpCollisionPostSolveFunc postSolve,
	cpCollisionSeparateFunc separate,
	void *data
){
	// Remove any old function so the new one will get added.
	cpSpaceRemoveCollisionHandler(space, a, b);
	
	cpCollisionHandler handler = {
		a, b,
		begin ? begin : alwaysCollide,
		preSolve ? preSolve : alwaysCollide,
		postSolve ? postSolve : nothing,
		separate ? separate : nothing,
		data
	};
	
	cpHashSetInsert(space->collFuncSet, CP_HASH_PAIR(a, b), &handler, NULL);
}

void
cpSpaceRemoveCollisionHandler(cpSpace *space, cpCollisionType a, cpCollisionType b)
{
	struct{cpCollisionType a, b;} ids = {a, b};
	cpCollisionHandler *old_handler = (cpCollisionHandler *) cpHashSetRemove(space->collFuncSet, CP_HASH_PAIR(a, b), &ids);
	cpfree(old_handler);
}

void
cpSpaceSetDefaultCollisionHandler(
	cpSpace *space,
	cpCollisionBeginFunc begin,
	cpCollisionPreSolveFunc preSolve,
	cpCollisionPostSolveFunc postSolve,
	cpCollisionSeparateFunc separate,
	void *data
){
	cpCollisionHandler handler = {
		0, 0,
		begin ? begin : alwaysCollide,
		preSolve ? preSolve : alwaysCollide,
		postSolve ? postSolve : nothing,
		separate ? separate : nothing,
		data
	};
	
	space->defaultHandler = handler;
}

#pragma mark Body, Shape, and Joint Management

#define cpAssertSpaceUnlocked(space) \
	cpAssert(!space->locked, \
		"This addition/removal cannot be done safely during a call to cpSpaceStep(). " \
		"Put these calls into a Post Step Callback." \
	);

static void
cpBodyAddShape(cpBody *body, cpShape *shape)
{
	shape->next = shape->body->shapesList;
	shape->body->shapesList = shape;
}

static void
cpBodyRemoveShape(cpBody *body, cpShape *shape)
{
	cpShape **prev_ptr = &body->shapesList;
	cpShape *node = body->shapesList;
	
	while(node && node != shape){
		prev_ptr = &node->next;
		node = node->next;
	}
	
	cpAssert(node, "Attempted to remove a shape from a body it was never attached to.");
	(*prev_ptr) = node->next;
}

static inline void
addShapeRaw(cpShape *shape, cpSpaceHash *hash)
{
	cpSpaceHashInsert(hash, shape, shape->hashid, shape->bb);
}

static inline void
removeShapeRaw(cpShape *shape, cpSpaceHash *hash)
{
	cpSpaceHashRemove(hash, shape, shape->hashid);
}

cpShape *
cpSpaceAddShape(cpSpace *space, cpShape *shape)
{
	cpBody *body = shape->body;
	if(!body || body == &space->staticBody) return cpSpaceAddStaticShape(space, shape);
	
	cpAssert(!cpHashSetFind(space->activeShapes->handleSet, shape->hashid, shape),
		"Cannot add the same shape more than once.");
	cpAssertSpaceUnlocked(space);
	
	cpBodyActivate(body);
	cpBodyAddShape(body, shape);
	addShapeRaw(shape, space->activeShapes);
		
	return shape;
}

static void
activateShapesTouchingShapeHelper(cpShape *shape, void *unused)
{
	cpBodyActivate(shape->body);
}

static void
activateShapesTouchingShape(cpSpace *space, cpShape *shape)
{
	// TODO this query should be more precise
	cpSpaceBBQuery(space, shape->bb, CP_ALL_LAYERS, CP_NO_GROUP, activateShapesTouchingShapeHelper, NULL);
}

cpShape *
cpSpaceAddStaticShape(cpSpace *space, cpShape *shape)
{
	cpAssert(!cpHashSetFind(space->staticShapes->handleSet, shape->hashid, shape),
		"Cannot add the same static shape more than once.");
	cpAssertSpaceUnlocked(space);
	
	if(!shape->body) shape->body = &space->staticBody;
	
	cpShapeCacheBB(shape);
	activateShapesTouchingShape(space, shape);
	addShapeRaw(shape, space->staticShapes);
	
	return shape;
}

cpBody *
cpSpaceAddBody(cpSpace *space, cpBody *body)
{
	cpAssertWarn(body->m != INFINITY, "Did you really mean to add an infinite mass body to the space?");
	cpAssert(!body->space, "Cannot add a body to a more than one space or to the same space twice.");
//	cpAssertSpaceUnlocked(space); This should be safe as long as it's not from an integration callback
	
	cpArrayPush(space->bodies, body);
	body->space = space;
	
	return body;
}

cpConstraint *
cpSpaceAddConstraint(cpSpace *space, cpConstraint *constraint)
{
	cpAssert(!cpArrayContains(space->constraints, constraint), "Cannot add the same constraint more than once.");
//	cpAssertSpaceUnlocked(space); This should be safe as long as its not from a constraint callback.
	
	if(!constraint->a) constraint->a = &space->staticBody;
	if(!constraint->b) constraint->b = &space->staticBody;
	
	cpBodyActivate(constraint->a);
	cpBodyActivate(constraint->b);
	cpArrayPush(space->constraints, constraint);
	
	return constraint;
}

typedef struct removalContext {
	cpSpace *space;
	cpShape *shape;
} removalContext;

// Hashset filter func to throw away old arbiters.
static cpBool
contactSetFilterRemovedShape(cpArbiter *arb, removalContext *context)
{
	if(context->shape == arb->private_a || context->shape == arb->private_b){
		arb->handler->separate(arb, context->space, arb->handler->data);
		cpArrayPush(context->space->pooledArbiters, arb);
		return cpFalse;
	}
	
	return cpTrue;
}

void
cpSpaceRemoveShape(cpSpace *space, cpShape *shape)
{
	cpBody *body = shape->body;
	if(cpBodyIsStatic(body)){
		cpSpaceRemoveStaticShape(space, shape);
		return;
	}

	cpBodyActivate(body);
	
	cpAssertSpaceUnlocked(space);
	cpAssertWarn(cpHashSetFind(space->activeShapes->handleSet, shape->hashid, shape),
		"Cannot remove a shape that was never added to the space. (Removed twice maybe?)");
	
	cpBodyRemoveShape(body, shape);
	
	removalContext context = {space, shape};
	cpHashSetFilter(space->contactSet, (cpHashSetFilterFunc)contactSetFilterRemovedShape, &context);
	removeShapeRaw(shape, space->activeShapes);
}

void
cpSpaceRemoveStaticShape(cpSpace *space, cpShape *shape)
{
	cpAssertWarn(cpHashSetFind(space->staticShapes->handleSet, shape->hashid, shape),
		"Cannot remove a static or sleeping shape that was never added to the space. (Removed twice maybe?)");
	cpAssertSpaceUnlocked(space);
	
	removalContext context = {space, shape};
	cpHashSetFilter(space->contactSet, (cpHashSetFilterFunc)contactSetFilterRemovedShape, &context);
	removeShapeRaw(shape, space->staticShapes);
	
	activateShapesTouchingShape(space, shape);
}

void
cpSpaceRemoveBody(cpSpace *space, cpBody *body)
{
	cpAssertWarn(body->space == space,
		"Cannot remove a body that was never added to the space. (Removed twice maybe?)");
	cpAssertSpaceUnlocked(space);
	
	cpBodyActivate(body);
	cpArrayDeleteObj(space->bodies, body);
	body->space = NULL;
}

void
cpSpaceRemoveConstraint(cpSpace *space, cpConstraint *constraint)
{
	cpAssertWarn(cpArrayContains(space->constraints, constraint),
		"Cannot remove a constraint that was never added to the space. (Removed twice maybe?)");
//	cpAssertSpaceUnlocked(space); Should be safe as long as its not from a constraint callback.
	
	cpBodyActivate(constraint->a);
	cpBodyActivate(constraint->b);
	cpArrayDeleteObj(space->constraints, constraint);
}

#pragma mark Post Step Functions

void
cpSpaceAddPostStepCallback(cpSpace *space, cpPostStepFunc func, void *obj, void *data)
{
	postStepCallback callback = {func, obj, data};
	cpHashSetInsert(space->postStepCallbacks, (cpHashValue)(size_t)obj, &callback, NULL);
}

static void
removeAndFreeShapeAndBody(cpShape *shape, cpSpace *space)
{
	cpSpaceRemoveShape(space, shape);
	cpShapeFree(shape);
}

void
cpSpacePostStepRemoveAndFreeShapeAndBody(cpSpace *space, cpShape *shape)
{
	cpSpaceAddPostStepCallback(space, (cpPostStepFunc)removeAndFreeShapeAndBody, shape, space);
}

#pragma mark Point Query Functions

typedef struct pointQueryContext {
	cpLayers layers;
	cpGroup group;
	cpSpacePointQueryFunc func;
	void *data;
} pointQueryContext;

static void 
pointQueryHelper(cpVect *point, cpShape *shape, pointQueryContext *context)
{
	if(
		!(shape->group && context->group == shape->group) && (context->layers&shape->layers) &&
		cpShapePointQuery(shape, *point)
	){
		context->func(shape, context->data);
	}
}

void
cpSpacePointQuery(cpSpace *space, cpVect point, cpLayers layers, cpGroup group, cpSpacePointQueryFunc func, void *data)
{
	pointQueryContext context = {layers, group, func, data};
	cpSpaceHashPointQuery(space->activeShapes, point, (cpSpaceHashQueryFunc)pointQueryHelper, &context);
	cpSpaceHashPointQuery(space->staticShapes, point, (cpSpaceHashQueryFunc)pointQueryHelper, &context);
}

static void
rememberLastPointQuery(cpShape *shape, cpShape **outShape)
{
	(*outShape) = shape;
}

cpShape *
cpSpacePointQueryFirst(cpSpace *space, cpVect point, cpLayers layers, cpGroup group)
{
	cpShape *shape = NULL;
	cpSpacePointQuery(space, point, layers, group, (cpSpacePointQueryFunc)rememberLastPointQuery, &shape);
	
	return shape;
}

void
cpSpaceEachBody(cpSpace *space, cpSpaceBodyIterator func, void *data)
{
	cpArray *bodies = space->bodies;
	
	for(int i=0; i<bodies->num; i++)
		func((cpBody *)bodies->arr[i], data);
}

#pragma mark Segment Query Functions

typedef struct segQueryContext {
	cpVect start, end;
	cpLayers layers;
	cpGroup group;
	cpSpaceSegmentQueryFunc func;
	cpBool anyCollision;
} segQueryContext;

static cpFloat
segQueryFunc(segQueryContext *context, cpShape *shape, void *data)
{
	cpSegmentQueryInfo info;
	
	if(
		!(shape->group && context->group == shape->group) && (context->layers&shape->layers) &&
		cpShapeSegmentQuery(shape, context->start, context->end, &info)
	){
		if(context->func){
			context->func(shape, info.t, info.n, data);
		}
		
		context->anyCollision = cpTrue;
	}
	
	return 1.0f;
}

cpBool
cpSpaceSegmentQuery(cpSpace *space, cpVect start, cpVect end, cpLayers layers, cpGroup group, cpSpaceSegmentQueryFunc func, void *data)
{
	segQueryContext context = {
		start, end,
		layers, group,
		func,
		cpFalse,
	};
	
	cpSpaceHashSegmentQuery(space->staticShapes, &context, start, end, 1.0f, (cpSpaceHashSegmentQueryFunc)segQueryFunc, data);
	cpSpaceHashSegmentQuery(space->activeShapes, &context, start, end, 1.0f, (cpSpaceHashSegmentQueryFunc)segQueryFunc, data);
	
	return context.anyCollision;
}

typedef struct segQueryFirstContext {
	cpVect start, end;
	cpLayers layers;
	cpGroup group;
} segQueryFirstContext;

static cpFloat
segQueryFirst(segQueryFirstContext *context, cpShape *shape, cpSegmentQueryInfo *out)
{
	cpSegmentQueryInfo info;// = {NULL, 1.0f, cpvzero};
	if(
		!(shape->group && context->group == shape->group) && (context->layers&shape->layers) &&
		cpShapeSegmentQuery(shape, context->start, context->end, &info)
	){
		if(info.t < out->t){
			out->shape = info.shape;
			out->t = info.t;
			out->n = info.n;
		}
		
		return info.t;
	}
	
	return 1.0f;
}

cpShape *
cpSpaceSegmentQueryFirst(cpSpace *space, cpVect start, cpVect end, cpLayers layers, cpGroup group, cpSegmentQueryInfo *out)
{
	cpSegmentQueryInfo info = {NULL, 1.0f, cpvzero};
	if(out){
		(*out) = info;
  } else {
		out = &info;
	}
	
	out->t = 1.0f;
	
	segQueryFirstContext context = {
		start, end,
		layers, group
	};
	
	cpSpaceHashSegmentQuery(space->staticShapes, &context, start, end, 1.0f, (cpSpaceHashSegmentQueryFunc)segQueryFirst, out);
	cpSpaceHashSegmentQuery(space->activeShapes, &context, start, end, out->t, (cpSpaceHashSegmentQueryFunc)segQueryFirst, out);
	
	return out->shape;
}

#pragma mark BB Query functions

typedef struct bbQueryContext {
	cpLayers layers;
	cpGroup group;
	cpSpaceBBQueryFunc func;
	void *data;
} bbQueryContext;

static void 
bbQueryHelper(cpBB *bb, cpShape *shape, bbQueryContext *context)
{
	if(
		!(shape->group && context->group == shape->group) && (context->layers&shape->layers) &&
		cpBBintersects(*bb, shape->bb)
	){
		context->func(shape, context->data);
	}
}

void
cpSpaceBBQuery(cpSpace *space, cpBB bb, cpLayers layers, cpGroup group, cpSpaceBBQueryFunc func, void *data)
{
	bbQueryContext context = {layers, group, func, data};
	cpSpaceHashQuery(space->activeShapes, &bb, bb, (cpSpaceHashQueryFunc)bbQueryHelper, &context);
	cpSpaceHashQuery(space->staticShapes, &bb, bb, (cpSpaceHashQueryFunc)bbQueryHelper, &context);
}

#pragma mark Spatial Hash Management

// Iterator function used for updating shape BBoxes.
static void
updateBBCache(cpShape *shape, void *unused)
{
	cpShapeCacheBB(shape);
}

void
cpSpaceResizeStaticHash(cpSpace *space, cpFloat dim, int count)
{
	cpSpaceHashResize(space->staticShapes, dim, count);
	cpSpaceHashRehash(space->staticShapes);
}

void
cpSpaceResizeActiveHash(cpSpace *space, cpFloat dim, int count)
{
	cpSpaceHashResize(space->activeShapes, dim, count);
}

void 
cpSpaceRehashStatic(cpSpace *space)
{
	cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)&updateBBCache, NULL);
	cpSpaceHashRehash(space->staticShapes);
}

#pragma mark Collision Detection Functions

static cpContactBufferHeader *
cpSpaceGetFreeContactBuffer(cpSpace *space)
{
	if(space->stamp - space->contactBuffersTail->stamp > cp_contact_persistence){
		cpContactBufferHeader *header = space->contactBuffersTail;
		space->contactBuffersTail = header->next;
		
		return cpContactBufferHeaderInit(header, space);
	} else {
		cpContactBufferHeader *header = cpSpaceAllocContactBuffer(space);
		return cpContactBufferHeaderInit(header, space);
	}
}

static void
cpSpacePushNewContactBuffer(cpSpace *space)
{
	cpContactBufferHeader *buffer = cpSpaceGetFreeContactBuffer(space);
	space->contactBuffersHead->next = buffer;
	space->contactBuffersHead = buffer;
}

static inline cpBool
queryReject(cpShape *a, cpShape *b)
{
	return
		// BBoxes must overlap
		!cpBBintersects(a->bb, b->bb)
		// Don't collide shapes attached to the same body.
		|| a->body == b->body
		// Don't collide objects in the same non-zero group
		|| (a->group && b->group && a->group == b->group)
		// Don't collide objects that don't share at least on layer.
		|| !(a->layers & b->layers);
}

// Callback from the spatial hash.
static void
queryFunc(cpShape *a, cpShape *b, cpSpace *space)
{
	// Reject any of the simple cases
	if(queryReject(a,b)) return;
	
	// Find the collision pair function for the shapes.
	struct{cpCollisionType a, b;} ids = {a->collision_type, b->collision_type};
	cpHashValue collHashID = CP_HASH_PAIR(a->collision_type, b->collision_type);
	cpCollisionHandler *handler = (cpCollisionHandler *)cpHashSetFind(space->collFuncSet, collHashID, &ids);
	
	cpBool sensor = a->sensor || b->sensor;
	if(sensor && handler == &space->defaultHandler) return;
	
	// Shape 'a' should have the lower shape type. (required by cpCollideShapes() )
	if(a->klass->type > b->klass->type){
		cpShape *temp = a;
		a = b;
		b = temp;
	}
	
	if(space->contactBuffersHead->numContacts + CP_MAX_CONTACTS_PER_ARBITER > CP_CONTACTS_BUFFER_SIZE){
		// contact buffer could overflow on the next collision, push a fresh one.
		cpSpacePushNewContactBuffer(space);
	}
	
	// Narrow-phase collision detection.
	cpContact *contacts = ((cpContactBuffer *)(space->contactBuffersHead))->contacts + space->contactBuffersHead->numContacts;
	int numContacts = cpCollideShapes(a, b, contacts);
	if(!numContacts) return; // Shapes are not colliding.
	space->contactBuffersHead->numContacts += numContacts;
	
	// Get an arbiter from space->contactSet for the two shapes.
	// This is where the persistant contact magic comes from.
	cpShape *shape_pair[] = {a, b};
	cpHashValue arbHashID = CP_HASH_PAIR((size_t)a, (size_t)b);
	cpArbiter *arb = (cpArbiter *)cpHashSetInsert(space->contactSet, arbHashID, shape_pair, space);
	cpArbiterUpdate(arb, contacts, numContacts, handler, a, b); // retains the contacts array
	
	// Call the begin function first if it's the first step
	if(arb->state == cpArbiterStateFirstColl && !handler->begin(arb, space, handler->data)){
		cpArbiterIgnore(arb); // permanently ignore the collision until separation
	}
	
	if(
		// Ignore the arbiter if it has been flagged
		(arb->state != cpArbiterStateIgnore) && 
		// Call preSolve
		handler->preSolve(arb, space, handler->data) &&
		// Process, but don't add collisions for sensors.
		!sensor
	){
		cpArrayPush(space->arbiters, arb);
	} else {
		space->contactBuffersHead->numContacts -= numContacts;
		arb->contacts = NULL;
		arb->numContacts = 0;
	}
	
	// Time stamp the arbiter so we know it was used recently.
	arb->stamp = space->stamp;
}

// Iterator for active/static hash collisions.
static void
active2staticIter(cpShape *shape, cpSpace *space)
{
	cpSpaceHashQuery(space->staticShapes, shape, shape->bb, (cpSpaceHashQueryFunc)queryFunc, space);
}

// Hashset filter func to throw away old arbiters.
static cpBool
contactSetFilter(cpArbiter *arb, cpSpace *space)
{
	if(space->sleepTimeThreshold != INFINITY){
		cpBody *a = arb->private_a->body;
		cpBody *b = arb->private_b->body;
		
		// both bodies are either static or sleeping
		cpBool sleepingNow =
			(cpBodyIsStatic(a) || cpBodyIsSleeping(a)) &&
			(cpBodyIsStatic(b) || cpBodyIsSleeping(b));
		
		if(sleepingNow){
			arb->state = cpArbiterStateSleep;
			return cpTrue;
		} else if(arb->state == cpArbiterStateSleep){
			// wake up the arbiter and continue as normal
			arb->state = cpArbiterStateNormal;
			// TODO is it possible that cpArbiterStateIgnore should be set here instead?
		}
	}
	
	cpTimestamp ticks = space->stamp - arb->stamp;
	
	// was used last frame, but not this one
	if(ticks >= 1 && arb->state != cpArbiterStateCached){
		arb->handler->separate(arb, space, arb->handler->data);
		arb->state = cpArbiterStateCached;
	}
	
	if(ticks >= cp_contact_persistence){
		arb->contacts = NULL;
		arb->numContacts = 0;
		
		cpArrayPush(space->pooledArbiters, arb);
		return cpFalse;
	}
	
	return cpTrue;
}

// Hashset filter func to call and throw away post step callbacks.
static void
postStepCallbackSetIter(postStepCallback *callback, cpSpace *space)
{
	callback->func(space, callback->obj, callback->data);
	cpfree(callback);
}

#pragma mark Sleeping Functions

// Chipmunk uses a data structure called a disjoint set forest.
// My attempts to find a way to splice circularly linked lists in
// constant time failed, and so I found this neat data structure instead.

static inline cpBody *
componentNodeRoot(cpBody *body)
{
	cpBody *parent = body->node.parent;
	
	if(!parent){
		return body;
	} else {
		// path compression, attaches this node directly to the root
		return (body->node.parent = componentNodeRoot(parent));
	}
}

static inline void
componentNodeMerge(cpBody *a_root, cpBody *b_root)
{
	if(a_root->node.rank < b_root->node.rank){
		a_root->node.parent = b_root;
	} else if(a_root->node.rank > b_root->node.rank){
		b_root->node.parent = a_root;
	} else if(a_root != b_root){
		b_root->node.parent = a_root;
		a_root->node.rank++;
	}
}

static inline void
componentActivate(cpBody *root)
{
	if(!cpBodyIsSleeping(root)) return;
	
	cpSpace *space = root->space;
	cpAssert(space, "Trying to activate a body that was never added to a space.");
	
	cpBody *body = root, *next;
	do {
		next = body->node.next;
		body->node.next = NULL;
		body->node.idleTime = 0.0f;
		cpArrayPush(space->bodies, body);
		
		for(cpShape *shape=body->shapesList; shape; shape=shape->next){
			removeShapeRaw(shape, space->staticShapes);
			addShapeRaw(shape, space->activeShapes);
		}
	} while((body = next) != root);
	
	cpArrayDeleteObj(space->sleepingComponents, root);
}

void
cpBodyActivate(cpBody *body)
{
	// Force the body to wake up even if it's not in a currently sleeping component
	// Like a body resting on or jointed to a rogue body.
	body->node.idleTime = 0.0f;
	
	cpBody *root = componentNodeRoot(body);
	if(root) componentActivate(root);
}

static inline void
mergeBodies(cpSpace *space, cpArray *components, cpArray *rogueBodies, cpBody *a, cpBody *b)
{
	// Don't merge with the static body
	if(cpBodyIsStatic(a) || cpBodyIsStatic(b)) return;
	
	cpBody *a_root = componentNodeRoot(a);
	cpBody *b_root = componentNodeRoot(b);
	
	cpBool a_sleep = cpBodyIsSleeping(a_root);
	cpBool b_sleep = cpBodyIsSleeping(b_root);
	
	if(a_sleep && b_sleep){
		return;
	} else if(a_sleep || b_sleep){
		componentActivate(a_root);
		componentActivate(b_root);
	} 
	
	// Add any rogue bodies (bodies not added to the space)
	if(!a->space) cpArrayPush(rogueBodies, a);
	if(!b->space) cpArrayPush(rogueBodies, b);
	
	componentNodeMerge(a_root, b_root);
}

static inline cpBool
componentActive(cpBody *root, cpFloat threshold)
{
	cpBody *body = root, *next;
	do {
		next = body->node.next;
		if(cpBodyIsRogue(body) || body->node.idleTime < threshold) return cpTrue;
	} while((body = next) != root);
	
	return cpFalse;
}

static inline void
addToComponent(cpBody *body, cpArray *components)
{
	// Check that the body is not already added to the component list
	if(body->node.next) return;
	cpBody *root = componentNodeRoot(body);
	
	cpBody *next = root->node.next;
	if(!next){
		// If the root isn't part of a list yet, then it hasn't been
		// added to the components list. Do that now.
		cpArrayPush(components, root);
		// Start the list
		body->node.next = root;
		root->node.next = body;
	} else if(root != body) {
		// Splice in body after the root.
		body->node.next = next;
		root->node.next = body;
	}
}

// TODO this function needs more commenting.
static void
processContactComponents(cpSpace *space, cpFloat dt)
{
	cpArray *bodies = space->bodies;
	cpArray *newBodies = cpArrayNew(bodies->num);
	cpArray *rogueBodies = cpArrayNew(16);
	cpArray *arbiters = space->arbiters;
	cpArray *constraints = space->constraints;
	cpArray *components = cpArrayNew(bodies->num/8);
	
	cpFloat dv = space->idleSpeedThreshold;
	cpFloat dvsq = (dv ? dv*dv : cpvdot(space->gravity, space->gravity)*dt*dt);
	// update idling
	for(int i=0; i<bodies->num; i++){
		cpBody *body = (cpBody*)bodies->arr[i];
		
		cpFloat thresh = (dvsq ? body->m*dvsq : 0.0f);
		body->node.idleTime = (cpBodyKineticEnergy(body) > thresh ? 0.0f : body->node.idleTime + dt);
	}
	
	// iterate graph edges and build forests
	for(int i=0; i<arbiters->num; i++){
		cpArbiter *arb = (cpArbiter*)arbiters->arr[i];
		mergeBodies(space, components, rogueBodies, arb->private_a->body, arb->private_b->body);
	}
	for(int j=0; j<constraints->num; j++){
		cpConstraint *constraint = (cpConstraint *)constraints->arr[j];
		mergeBodies(space, components, rogueBodies, constraint->a, constraint->b);
	}
	
	// iterate bodies and add them to their components
	for(int i=0; i<bodies->num; i++)
		addToComponent((cpBody*)bodies->arr[i], components);
	for(int i=0; i<rogueBodies->num; i++)
		addToComponent((cpBody*)rogueBodies->arr[i], components);
	
	// iterate components, copy or deactivate
	for(int i=0; i<components->num; i++){
		cpBody *root = (cpBody*)components->arr[i];
		if(componentActive(root, space->sleepTimeThreshold)){
			cpBody *body = root, *next;
			do {
				next = body->node.next;
				
				if(!cpBodyIsRogue(body)) cpArrayPush(newBodies, body);
				body->node.next = NULL;
				body->node.parent = NULL;
				body->node.rank = 0;
			} while((body = next) != root);
		} else {
			cpBody *body = root, *next;
			do {
				next = body->node.next;
				
				for(cpShape *shape = body->shapesList; shape; shape = shape->next){
					removeShapeRaw(shape, space->activeShapes);
					addShapeRaw(shape, space->staticShapes);
				}
			} while((body = next) != root);
			
			cpArrayPush(space->sleepingComponents, root);
		}
	}
	
	space->bodies = newBodies;
	cpArrayFree(bodies);
	cpArrayFree(rogueBodies);
	cpArrayFree(components);
}

#pragma mark All Important cpSpaceStep() Function

void
cpSpaceStep(cpSpace *space, cpFloat dt)
{
	if(!dt) return; // don't step if the timestep is 0!
	
	cpFloat dt_inv = 1.0f/dt;

	cpArray *bodies = space->bodies;
	cpArray *constraints = space->constraints;
	
	space->locked = cpTrue;
	
	// Empty the arbiter list.
	space->arbiters->num = 0;

	// Integrate positions.
	for(int i=0; i<bodies->num; i++){
		cpBody *body = (cpBody *)bodies->arr[i];
		body->position_func(body, dt);
	}
	
	// Pre-cache BBoxes and shape data.
	cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)updateBBCache, NULL);
	
	// Collide!
	cpSpacePushNewContactBuffer(space);
	if(space->staticShapes->handleSet->entries)
		cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)active2staticIter, space);
	cpSpaceHashQueryRehash(space->activeShapes, (cpSpaceHashQueryFunc)queryFunc, space);
	
	// If body sleeping is enabled, do that now.
	if(space->sleepTimeThreshold != INFINITY){
		processContactComponents(space, dt);
		bodies = space->bodies; // rebuilt by processContactComponents()
	}
	
	// Clear out old cached arbiters and dispatch untouch functions
	cpHashSetFilter(space->contactSet, (cpHashSetFilterFunc)contactSetFilter, space);

	// Prestep the arbiters.
	cpArray *arbiters = space->arbiters;
	for(int i=0; i<arbiters->num; i++)
		cpArbiterPreStep((cpArbiter *)arbiters->arr[i], dt_inv);

	// Prestep the constraints.
	for(int i=0; i<constraints->num; i++){
		cpConstraint *constraint = (cpConstraint *)constraints->arr[i];
		constraint->klass->preStep(constraint, dt, dt_inv);
	}

	for(int i=0; i<space->elasticIterations; i++){
		for(int j=0; j<arbiters->num; j++)
			cpArbiterApplyImpulse((cpArbiter *)arbiters->arr[j], 1.0f);
			
		for(int j=0; j<constraints->num; j++){
			cpConstraint *constraint = (cpConstraint *)constraints->arr[j];
			constraint->klass->applyImpulse(constraint);
		}
	}

	// Integrate velocities.
	cpFloat damping = cpfpow(1.0f/space->damping, -dt);
	for(int i=0; i<bodies->num; i++){
		cpBody *body = (cpBody *)bodies->arr[i];
		body->velocity_func(body, space->gravity, damping, dt);
	}

	for(int i=0; i<arbiters->num; i++)
		cpArbiterApplyCachedImpulse((cpArbiter *)arbiters->arr[i]);
	
	// run the old-style elastic solver if elastic iterations are disabled
	cpFloat elasticCoef = (space->elasticIterations ? 0.0f : 1.0f);
	
	// Run the impulse solver.
	for(int i=0; i<space->iterations; i++){
		for(int j=0; j<arbiters->num; j++)
			cpArbiterApplyImpulse((cpArbiter *)arbiters->arr[j], elasticCoef);
			
		for(int j=0; j<constraints->num; j++){
			cpConstraint *constraint = (cpConstraint *)constraints->arr[j];
			constraint->klass->applyImpulse(constraint);
		}
	}
	
	space->locked = cpFalse;
	
	// run the post solve callbacks
	for(int i=0; i<arbiters->num; i++){
		cpArbiter *arb = (cpArbiter *) arbiters->arr[i];
		
		cpCollisionHandler *handler = arb->handler;
		handler->postSolve(arb, space, handler->data);
		
		arb->state = cpArbiterStateNormal;
	}
	
	// Run the post step callbacks
	while(space->postStepCallbacks->entries){
		cpHashSet *callbacks = space->postStepCallbacks;
		space->postStepCallbacks = cpHashSetNew(0, (cpHashSetEqlFunc)postStepFuncSetEql, (cpHashSetTransFunc)postStepFuncSetTrans);
		cpHashSetEach(callbacks, (cpHashSetIterFunc)postStepCallbackSetIter, space);
	}
	
	// Increment the stamp.
	space->stamp++;
}
