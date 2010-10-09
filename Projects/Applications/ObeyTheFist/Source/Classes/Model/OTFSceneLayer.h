
#import <UIKit/UIKit.h>
#import <Cocos2D/Cocos2D.h>
#import <Chipmunk/Chipmunk.h>

@interface OTFSceneLayer : CCLayer {
	cpSpace *space;
}

	// returns a Scene that contains the HelloWorld as the only child
	+(id) scene;

	-(void) step:(ccTime)dt;
	
	-(void) addNewSpriteX:(float)x y:(float)y;

@end
