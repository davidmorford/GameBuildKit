
/*!
@project	Cocos2D
@abstract	cocos2d for iPhone: http://www.cocos2d-iphone.org
@discussion	The programming guide is hosted here: http://www.cocos2d-iphone.org/wiki/doku.php/prog_guide:index
@copyright	(c) 2008-2010 Ricardo Quesada
@license	Documents/Cocos2D-License.md
@todo		A native english speaker should check the grammar. We need your help!
*/

// 0x00 HI ME LO
// 00   00 99 05
#define COCOS2D_VERSION 0x00009905

#import <Availability.h>

// should be included first
#import <Cocos2D/ccConfig.h>

#import <Cocos2D/CCActionManager.h>
#import <Cocos2D/CCAction.h>
#import <Cocos2D/CCActionInstant.h>
#import <Cocos2D/CCActionInterval.h>
#import <Cocos2D/CCActionEase.h>
#import <Cocos2D/CCActionCamera.h>
#import <Cocos2D/CCActionTween.h>
#import <Cocos2D/CCActionEase.h>
#import <Cocos2D/CCActionTiledGrid.h>
#import <Cocos2D/CCActionGrid3D.h>
#import <Cocos2D/CCActionGrid.h>
#import <Cocos2D/CCActionProgressTimer.h>
#import <Cocos2D/CCActionPageTurn3D.h>

#import <Cocos2D/CCSprite.h>
#import <Cocos2D/CCSpriteFrame.h>
#import <Cocos2D/CCSpriteSheet.h>
#import <Cocos2D/CCSpriteBatchNode.h>
#import <Cocos2D/CCSpriteFrameCache.h>

#import <Cocos2D/CCLabelTTF.h>
#import <Cocos2D/CCLabelBMFont.h>
#import <Cocos2D/CCLabelAtlas.h>

#import <Cocos2D/CCParticleSystem.h>
#import <Cocos2D/CCParticleSystemPoint.h>
#import <Cocos2D/CCParticleSystemQuad.h>
#import <Cocos2D/CCParticleExamples.h>

#import <Cocos2D/CCTexture2D.h>
#import <Cocos2D/CCTexturePVR.h>
#import <Cocos2D/CCTextureCache.h>
#import <Cocos2D/CCTextureAtlas.h>

#import <Cocos2D/CCTransition.h>
#import <Cocos2D/CCTransitionPageTurn.h>
#import <Cocos2D/CCTransitionRadial.h>

#import <Cocos2D/CCTMXTiledMap.h>
#import <Cocos2D/CCTMXLayer.h>
#import <Cocos2D/CCTMXObjectGroup.h>
#import <Cocos2D/CCTMXXMLParser.h>
#import <Cocos2D/CCTileMapAtlas.h>

#import <Cocos2D/CCLayer.h>
#import <Cocos2D/CCMenu.h>
#import <Cocos2D/CCMenuItem.h>
#import <Cocos2D/CCDrawingPrimitives.h>
#import <Cocos2D/CCScene.h>
#import <Cocos2D/CCScheduler.h>
#import <Cocos2D/CCBlockSupport.h>
#import <Cocos2D/CCCamera.h>
#import <Cocos2D/CCProtocols.h>
#import <Cocos2D/CCNode.h>
#import <Cocos2D/CCDirector.h>
#import <Cocos2D/CCAtlasNode.h>
#import <Cocos2D/CCGrabber.h>
#import <Cocos2D/CCGrid.h>
#import <Cocos2D/CCParallaxNode.h>
#import <Cocos2D/CCRenderTexture.h>
#import <Cocos2D/CCMotionStreak.h>
#import <Cocos2D/CCConfiguration.h>

#pragma mark Macros

#import <Cocos2D/ccTypes.h>
#import <Cocos2D/ccMacros.h>


#pragma mark Platform

#import <Cocos2D/CCGL.h>
#import <Cocos2D/CCNS.h>

#ifdef __IPHONE_OS_VERSION_MAX_ALLOWED
#import <Cocos2D/CCTouchDispatcher.h>
#import <Cocos2D/CCTouchDelegateProtocol.h>
#import <Cocos2D/CCTouchHandler.h>
#import <Cocos2D/EAGLView.h>
#import <Cocos2D/CCDirectorIOS.h>

#elif defined(__MAC_OS_X_VERSION_MAX_ALLOWED)
#import <Cocos2D/Mac/MacGLView.h>
#import <Cocos2D/Mac/CCDirectorMac.h>
#endif


#pragma mark helper files

#import <Cocos2D/OpenGL_Internal.h>
#import <Cocos2D/CCFileUtils.h>
#import <Cocos2D/CGPointExtension.h>
#import <Cocos2D/ccCArray.h>
#import <Cocos2D/CCArray.h>
#import <Cocos2D/ccUtils.h>

#if CC_ENABLE_PROFILERS
#import <Cocos2D/CCProfiling.h>
#endif

#import <Cocos2D/CCCompatibility.h>
#import <Cocos2D/Cocos2DLibrary.h>
