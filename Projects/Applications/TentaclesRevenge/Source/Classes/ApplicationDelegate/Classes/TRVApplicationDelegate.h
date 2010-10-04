
/*!
@project	TentaclesRevenge
@header		TRVApplicationDelegate.h
*/

#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>
#import <NavigatorKit/NavigatorKit.h>
#import <Cocos2D/Cocos2D.h>

@class TRVMasterViewController, TRVDetailViewController;

/*!
@class TRVApplicationDelegate
@superclass NSObject <UIApplicationDelegate, NKNavigatorDelegate>
@abstract
@discussion
*/
@interface TRVApplicationDelegate : NSObject <UIApplicationDelegate, NKNavigatorDelegate>

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) UISplitViewController *splitViewController;

#pragma mark -

+(TRVApplicationDelegate *) sharedApplicationDelegate;

#pragma mark -

@property (nonatomic, readonly) NSString *applicationDocumentsDirectoryPath;

@end
