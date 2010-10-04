
#import <Cocos2D/Cocos2DLibrary.h>
#import <Foundation/NSString.h>

static NSString *version = @"cocos2d v0.99.5-beta3";

NSString * 
Cocos2DLibraryGetVersion() {
	return version;
}

NSString * 
cocos2dVersion() {	
	return version;
}
