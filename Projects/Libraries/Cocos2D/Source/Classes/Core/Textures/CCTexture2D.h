
/*!
@header Texture2D.h
@abstract: Creates OpenGL 2D textures from images or text.
@version: 1.6
@copyright (C) 2008 Apple Inc. All Rights Reserved.
*/

#import <Availability.h>

#ifdef __IPHONE_OS_VERSION_MAX_ALLOWED
#import <UIKit/UIKit.h>
#endif

#import <Foundation/Foundation.h>
#import <Cocos2D/CCGL.h>
#import <Cocos2D/CCNS.h>

//CONSTANTS:

/*! 
@typedef CCTexture2DPixelFormat
@abstract Possible texture pixel formats
*/
typedef enum {
	kCCTexture2DPixelFormat_Automatic = 0,
	//! 32-bit texture: RGBA8888
	kCCTexture2DPixelFormat_RGBA8888,
	//! 16-bit texture: used with images that have alpha pre-multiplied
	kCCTexture2DPixelFormat_RGB565,
	//! 8-bit textures used as masks
	kCCTexture2DPixelFormat_A8,
	//! 16-bit textures: RGBA4444
	kCCTexture2DPixelFormat_RGBA4444,
	//! 16-bit textures: RGB5A1
	kCCTexture2DPixelFormat_RGB5A1,	

	//! Default texture format: RGBA8888
	kCCTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_RGBA8888,

	// backward compatibility stuff
	kTexture2DPixelFormat_Automatic = kCCTexture2DPixelFormat_Automatic,
	kTexture2DPixelFormat_RGBA8888 = kCCTexture2DPixelFormat_RGBA8888,
	kTexture2DPixelFormat_RGB565 = kCCTexture2DPixelFormat_RGB565,
	kTexture2DPixelFormat_A8 = kCCTexture2DPixelFormat_A8,
	kTexture2DPixelFormat_RGBA4444 = kCCTexture2DPixelFormat_RGBA4444,
	kTexture2DPixelFormat_RGB5A1 = kCCTexture2DPixelFormat_RGB5A1,
	kTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_Default
	
} CCTexture2DPixelFormat;

//CLASS INTERFACES:

/*!
@class CCTexture2D
@abstract This class allows to easily create OpenGL 2D textures from images, text or raw data.
* The created CCTexture2D object will always have power-of-two dimensions. 
* Depending on how you create the CCTexture2D object, the actual image area of the texture might be smaller than the texture dimensions i.e. "contentSize" != (pixelsWide, pixelsHigh) and (maxS, maxT) != (1.0, 1.0).
* Be aware that the content of the generated textures will be upside-down!
*/
@interface CCTexture2D : NSObject {
	GLuint						name_;
	CGSize						size_;
	NSUInteger					width_,
								height_;
	CCTexture2DPixelFormat		format_;
	GLfloat						maxS_,
								maxT_;
	BOOL						hasPremultipliedAlpha_;
}
/** Intializes with a texture2d with data */
- (id) initWithData:(const void*)data pixelFormat:(CCTexture2DPixelFormat)pixelFormat pixelsWide:(NSUInteger)width pixelsHigh:(NSUInteger)height contentSize:(CGSize)size;

/** pixel format of the texture */
@property(nonatomic,readonly) CCTexture2DPixelFormat pixelFormat;
/** width in pixels */
@property(nonatomic,readonly) NSUInteger pixelsWide;
/** hight in pixels */
@property(nonatomic,readonly) NSUInteger pixelsHigh;

/** texture name */
@property(nonatomic,readonly) GLuint name;

/** returns content size of the texture in pixels */
@property(nonatomic,readonly, nonatomic) CGSize contentSizeInPixels;

/** texture max S */
@property(nonatomic,readwrite) GLfloat maxS;
/** texture max T */
@property(nonatomic,readwrite) GLfloat maxT;
/** whether or not the texture has their Alpha premultiplied */
@property(nonatomic,readonly) BOOL hasPremultipliedAlpha;

/** returns the content size of the texture in points */
-(CGSize) contentSize;
@end

/**
Drawing extensions to make it easy to draw basic quads using a CCTexture2D object.
These functions require GL_TEXTURE_2D and both GL_VERTEX_ARRAY and GL_TEXTURE_COORD_ARRAY client states to be enabled.
*/
@interface CCTexture2D (Drawing)
/** draws a texture at a given point */
- (void) drawAtPoint:(CGPoint)point;
/** draws a texture inside a rect */
- (void) drawInRect:(CGRect)rect;
@end

/**
Extensions to make it easy to create a CCTexture2D object from an image file.
Note that RGBA type textures will have their alpha premultiplied - use the blending mode (GL_ONE, GL_ONE_MINUS_SRC_ALPHA).
*/
@interface CCTexture2D (Image)
/** Initializes a texture from a UIImage object */
#ifdef __IPHONE_OS_VERSION_MAX_ALLOWED
- (id) initWithImage:(UIImage *)uiImage;
#elif defined(__MAC_OS_X_VERSION_MAX_ALLOWED)
- (id) initWithImage:(CGImageRef)cgImage;
#endif
@end

/**
Extensions to make it easy to create a CCTexture2D object from a string of text.
Note that the generated textures are of type A8 - use the blending mode (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA).
*/
@interface CCTexture2D (Text)
/** Initializes a texture from a string with dimensions, alignment, font name and font size */
- (id) initWithString:(NSString*)string dimensions:(CGSize)dimensions alignment:(CCTextAlignment)alignment fontName:(NSString*)name fontSize:(CGFloat)size;
/** Initializes a texture from a string with font name and font size */
- (id) initWithString:(NSString*)string fontName:(NSString*)name fontSize:(CGFloat)size;
@end


/**
 Extensions to make it easy to create a CCTexture2D object from a PVRTC file
 Note that the generated textures don't have their alpha premultiplied - use the blending mode (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA).
 */
@interface CCTexture2D (PVRSupport)
/** Initializes a texture from a PVR Texture Compressed (PVRTC) buffer
 *
 * IMPORTANT: This method is only defined on iOS. It is not supported on the Mac version.
 */
#ifdef __IPHONE_OS_VERSION_MAX_ALLOWED
-(id) initWithPVRTCData: (const void*)data level:(int)level bpp:(int)bpp hasAlpha:(BOOL)hasAlpha length:(int)length;
#endif // __IPHONE_OS_VERSION_MAX_ALLOWED
/** Initializes a texture from a PVR file.
 
 Supported PVR formats:
 - BGRA 8888
 - RGBA 8888
 - RGBA 4444
 - RGBA 5551
 - RBG 565
 - A 8
 - I 8
 - AI 8
 - PVRTC 2BPP
 - PVRTC 4BPP
 
 By default PVR images are treated as if they alpha channel is NOT premultiplied. You can override this behavior with this class method:
 - PVRImagesHavePremultipliedAlpha:(BOOL)haveAlphaPremultiplied;
 
 IMPORTANT: This method is only defined on iOS. It is not supported on the Mac version.
 
 */
-(id) initWithPVRFile: (NSString*) file;

/** treats (or not) PVR files as if they have alpha premultiplied.
 Since it is impossible to know at runtime if the PVR images have the alpha channel premultiplied, it is
 possible load them as if they have (or not) the alpha channel premultiplied.
 
 By default it is disabled by default.
 
 @since v0.99.5
 */
+(void) PVRImagesHavePremultipliedAlpha:(BOOL)haveAlphaPremultiplied;
@end

/**
 Extension to set the Min / Mag filter
 */
typedef struct _ccTexParams {
	GLuint	minFilter;
	GLuint	magFilter;
	GLuint	wrapS;
	GLuint	wrapT;
} ccTexParams;

@interface CCTexture2D (GLFilter)
/** sets the min filter, mag filter, wrap s and wrap t texture parameters.
 If the texture size is NPOT (non power of 2), then in can only use GL_CLAMP_TO_EDGE in GL_TEXTURE_WRAP_{S,T}.
 @since v0.8
 */
-(void) setTexParameters: (ccTexParams*) texParams;

/** sets antialias texture parameters:
  - GL_TEXTURE_MIN_FILTER = GL_LINEAR
  - GL_TEXTURE_MAG_FILTER = GL_LINEAR

 @since v0.8
 */
- (void) setAntiAliasTexParameters;

/** sets alias texture parameters:
  - GL_TEXTURE_MIN_FILTER = GL_NEAREST
  - GL_TEXTURE_MAG_FILTER = GL_NEAREST
 
 @since v0.8
 */
- (void) setAliasTexParameters;


/** Generates mipmap images for the texture.
 It only works if the texture size is POT (power of 2).
 @since v0.99.0
 */
-(void) generateMipmap;


@end

@interface CCTexture2D (PixelFormat)
/** sets the default pixel format for UIImages that contains alpha channel.
 If the UIImage contains alpha channel, then the options are:
	- generate 32-bit textures: kCCTexture2DPixelFormat_RGBA8888 (default one)
	- generate 16-bit textures: kCCTexture2DPixelFormat_RGBA4444
	- generate 16-bit textures: kCCTexture2DPixelFormat_RGB5A1
	- generate 16-bit textures: kCCTexture2DPixelFormat_RGB565
	- generate 8-bit textures: kCCTexture2DPixelFormat_A8 (only use it if you use just 1 color)

 How does it work ?
   - If the image is an RGBA (with Alpha) then the default pixel format will be used (it can be a 8-bit, 16-bit or 32-bit texture)
   - If the image is an RGB (without Alpha) then an RGB565 texture will be used (16-bit texture)
 
 This parameter is not valid for PVR images.
 
 @since v0.8
 */
+(void) setDefaultAlphaPixelFormat:(CCTexture2DPixelFormat)format;

/** returns the alpha pixel format
 @since v0.8
 */
+(CCTexture2DPixelFormat) defaultAlphaPixelFormat;
@end





