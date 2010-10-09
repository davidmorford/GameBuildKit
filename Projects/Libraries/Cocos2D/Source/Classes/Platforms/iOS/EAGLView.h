
/*!
@header		EAGLView.h
@abstract	Convenience class that wraps the CAEAGLLayer from CoreAnimation into a UIView subclass.
@version	1.3
@copyright	(C) 2008 Apple Inc. All Rights Reserved.
*/

#import <Availability.h>
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <Cocos2D/ESRenderer.h>

@class EAGLView;

@protocol EAGLTouchDelegate <NSObject>
-(void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;
@end

#pragma mark -

/*!  EAGLView Class.
 * This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
 * The view content is basically an EAGL surface you render your OpenGL scene into.
 * Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
 */
@interface EAGLView : UIView {
    id <ESRenderer> renderer_;	
	EAGLContext *context_; // weak ref

	NSString *pixelformat_;
	GLuint depthFormat_;
	BOOL preserveBackbuffer_;

	CGSize size_;
	BOOL discardFramebufferSupported_;
	id <EAGLTouchDelegate> touchDelegate_;
}

/*! creates an initializes an EAGLView with a frame and 0-bit depth buffer, and a RGB565 color buffer */
+(id) viewWithFrame:(CGRect)frame;

/*! creates an initializes an EAGLView with a frame, a color buffer format, and 0-bit depth buffer */
+(id) viewWithFrame:(CGRect)frame pixelFormat:(NSString *)format;

/*! creates an initializes an EAGLView with a frame, a color buffer format, and a depth buffer format */
+(id) viewWithFrame:(CGRect)frame pixelFormat:(NSString *)format depthFormat:(GLuint)depth preserveBackbuffer:(BOOL)retained;


/*! Initializes an EAGLView with a frame and 0-bit depth buffer, and a RGB565 color buffer */
-(id) initWithFrame:(CGRect)frame;  //These also set the current context

/*! Initializes an EAGLView with a frame, a color buffer format, and 0-bit depth buffer */
-(id) initWithFrame:(CGRect)frame pixelFormat:(NSString *)format;

/*! Initializes an EAGLView with a frame, a color buffer format, and a depth buffer format */
-(id) initWithFrame:(CGRect)frame pixelFormat:(NSString *)format depthFormat:(GLuint)depth preserveBackbuffer:(BOOL)retained;

/*! pixel format: it could be RGBA8 (32-bit) or RGB565 (16-bit) */
@property (nonatomic, readonly) NSString *pixelFormat;

/*! depth format of the render buffer: 0, 16 or 24 bits*/
@property (nonatomic, readonly) GLuint depthFormat;

/*! returns surface size in pixels */
@property (nonatomic, readonly) CGSize surfaceSize;

/*! OpenGL context */
@property (nonatomic, readonly) EAGLContext *context;

/*! touch delegate */
@property (nonatomic, readwrite, assign) id <EAGLTouchDelegate> touchDelegate;

/*! EAGLView uses double-buffer. This method swaps the buffers */
-(void) swapBuffers;

-(CGPoint) convertPointFromViewToSurface:(CGPoint)point;
-(CGRect) convertRectFromViewToSurface:(CGRect)rect;

@end
