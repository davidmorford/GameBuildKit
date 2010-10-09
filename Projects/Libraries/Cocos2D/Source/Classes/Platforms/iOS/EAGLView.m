
#import <Availability.h>
#import <QuartzCore/QuartzCore.h>
#import <Cocos2D/EAGLView.h>
#import <Cocos2D/ES1Renderer.h>
#import <Cocos2D/CCDirector.h>
#import <Cocos2D/ccMacros.h>
#import <Cocos2D/CCConfiguration.h>
#import <Cocos2D/OpenGL_Internal.h>

@interface EAGLView (Private)
-(BOOL) setupSurface;
@end

@implementation EAGLView

@synthesize surfaceSize = size_;
@synthesize pixelFormat = pixelformat_, depthFormat = depthFormat_;
@synthesize touchDelegate = touchDelegate_;
@synthesize context = context_;

+(Class) layerClass {
	return [CAEAGLLayer class];
}

#pragma mark -

+(id) viewWithFrame:(CGRect)frame {
	return [[[self alloc] initWithFrame:frame] autorelease];
}

+(id) viewWithFrame:(CGRect)frame pixelFormat:(NSString *)format {
	return [[[self alloc] initWithFrame:frame pixelFormat:format] autorelease];
}

+(id) viewWithFrame:(CGRect)frame pixelFormat:(NSString *)format depthFormat:(GLuint)depth preserveBackbuffer:(BOOL)retained {
	return [[[self alloc] initWithFrame:frame pixelFormat:format depthFormat:depth preserveBackbuffer:retained] autorelease];
}

#pragma mark -

-(id) initWithFrame:(CGRect)frame {
	return [self initWithFrame:frame pixelFormat:kEAGLColorFormatRGB565 depthFormat:0 preserveBackbuffer:NO];
}

-(id) initWithFrame:(CGRect)frame pixelFormat:(NSString *)format {
	return [self initWithFrame:frame pixelFormat:format depthFormat:0 preserveBackbuffer:NO];
}

-(id) initWithFrame:(CGRect)frame pixelFormat:(NSString *)format depthFormat:(GLuint)depth preserveBackbuffer:(BOOL)retained {
	if ((self = [super initWithFrame:frame])) {
		pixelformat_        = format;
		depthFormat_        = depth;
		preserveBackbuffer_ = retained;
		size_               = frame.size;

		if (![self setupSurface]) {
			[self release];
			return nil;
		}
	}

	return self;
}

#pragma mark -

-(id) initWithCoder:(NSCoder *)aDecoder {
	if ( (self = [super initWithCoder:aDecoder]) ) {

		CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[self layer];

		pixelformat_ = kEAGLColorFormatRGB565;
		depthFormat_ = 0; // GL_DEPTH_COMPONENT24_OES;
		size_ = [eaglLayer bounds].size;

		if (![self setupSurface]) {
			[self release];
			return nil;
		}
	}

	return self;
}

#pragma mark -

-(BOOL) setupSurface {
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber   numberWithBool:preserveBackbuffer_], kEAGLDrawablePropertyRetainedBacking,
									pixelformat_, kEAGLDrawablePropertyColorFormat, nil];


	renderer_ = [[ES1Renderer alloc] initWithDepthFormat:depthFormat_];
	if (!renderer_) {
		return NO;
	}

	context_ = [renderer_ context];
	[context_ renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eaglLayer];

	discardFramebufferSupported_ = [[CCConfiguration sharedConfiguration] supportsDiscardFramebuffer];

	return YES;
}

-(void) layoutSubviews {
	[renderer_ resizeFromLayer:(CAEAGLLayer *)self.layer];
	size_ = [renderer_ backingSize];

	// Issue #914 #924
	CCDirector *director = [CCDirector sharedDirector];
	[director reshapeProjection:size_];

	// Avoid flicker. Issue #350
	[director drawScene];
}

-(void) swapBuffers {
	// IMPORTANT:
	// - preconditions
	//	-> context_ MUST be the OpenGL context
	//	-> renderBuffer_ must be the the RENDER BUFFER

#if defined(__IPHONE_4_0)
	if (discardFramebufferSupported_ && depthFormat_) {
		GLenum attachments[] = { GL_DEPTH_ATTACHMENT_OES };
		glDiscardFramebufferEXT(GL_FRAMEBUFFER_OES, 1, attachments);
	}
#endif // __IPHONE_4_0

	if (![context_ presentRenderbuffer:GL_RENDERBUFFER_OES]) {
		CCLOG(@"cocos2d: Failed to swap renderbuffer in %s\n", __FUNCTION__);
	}

#if COCOS2D_DEBUG
	CHECK_GL_ERROR();
#endif
}

#pragma mark Point conversion

-(CGPoint) convertPointFromViewToSurface:(CGPoint)point {
	CGRect bounds = [self bounds];

	return CGPointMake((point.x - bounds.origin.x) / bounds.size.width * size_.width, (point.y - bounds.origin.y) / bounds.size.height * size_.height);
}

-(CGRect) convertRectFromViewToSurface:(CGRect)rect {
	CGRect bounds = [self bounds];

	return CGRectMake((rect.origin.x - bounds.origin.x) / bounds.size.width * size_.width, (rect.origin.y - bounds.origin.y) / bounds.size.height * size_.height, rect.size.width / bounds.size.width * size_.width, rect.size.height / bounds.size.height * size_.height);
}

#pragma mark Touch Delegate

/*!
@abstract Pass the touches to the superview
*/
-(void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	if (touchDelegate_) {
		[touchDelegate_ touchesBegan:touches withEvent:event];
	}
}

-(void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	if (touchDelegate_) {
		[touchDelegate_ touchesMoved:touches withEvent:event];
	}
}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	if (touchDelegate_) {
		[touchDelegate_ touchesEnded:touches withEvent:event];
	}
}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	if (touchDelegate_) {
		[touchDelegate_ touchesCancelled:touches withEvent:event];
	}
}

#pragma mark -

-(void) dealloc {
	CCLOGINFO(@"cocos2d: deallocing %@", self);
	[renderer_ release];
	[super dealloc];
}

@end
