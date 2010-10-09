
/*!
@project	ObeyTheFist
@header		OTFApplicationDelegate.h
@copyright	(c) 2010, Semantap
@created	10/9/10: David
*/

#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>
#import <NavigatorKit/NavigatorKit.h>
#import <Cocos2D/Cocos2D.h>

@class OTFMasterViewController, OTFDetailViewController;

/*!
@class OTFApplicationDelegate
@superclass NSObject <UIApplicationDelegate, NKNavigatorDelegate>
@abstract
@discussion
*/
@interface OTFApplicationDelegate : NSObject <UIApplicationDelegate, NKNavigatorDelegate>

	@property (nonatomic, retain) UIWindow *window;

	#pragma mark -

	+(OTFApplicationDelegate *) sharedApplicationDelegate;

	#pragma mark -

	@property (nonatomic, readonly) NSString *applicationDocumentsDirectoryPath;

@end
