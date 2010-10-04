
/*!
File: PVRTexture.h
Abstract: The PVRTexture class is responsible for loading .pvr files.
Version: 1.0
*/

#import <Foundation/Foundation.h>

#import <Cocos2D/CCGL.h>
#import <Cocos2D/CCTextureCache.h>
#import <Cocos2D/CCTexture2D.h>
#import <Cocos2D/ccCArray.h>

#pragma mark -
#pragma mark CCTextureCache PVR extension

#pragma mark -
#pragma mark CCTexture2D PVR extension


#pragma mark -
#pragma mark CCTexturePVR

@interface CCTexturePVR : NSObject
{
	ccArray *imageData_;
	
	
	unsigned int	tableFormatIndex_;
	uint32_t width_, height_;
	GLuint	name_;
	BOOL hasAlpha_;
	
	// cocos2d integration
	BOOL retainName_;
}

- (id)initWithContentsOfFile:(NSString *)path;
- (id)initWithContentsOfURL:(NSURL *)url;
+ (id)pvrTextureWithContentsOfFile:(NSString *)path;
+ (id)pvrTextureWithContentsOfURL:(NSURL *)url;

@property (nonatomic,readonly) GLuint name;
@property (nonatomic,readonly) uint32_t width;
@property (nonatomic,readonly) uint32_t height;
@property (nonatomic,readonly) BOOL hasAlpha;

// cocos2d integration
@property (nonatomic,readwrite) BOOL retainName;

@end


