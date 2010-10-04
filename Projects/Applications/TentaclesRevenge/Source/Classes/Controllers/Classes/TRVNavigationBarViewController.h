
/*!
@project	TentaclesRevenge
@header     TRVNavigationBarViewController.h
@created	10/3/10
*/

#import <UIKit/UIKit.h>
#import <NavigatorKit/NavigatorKit.h>

/*!
@class TRVNavigationBarViewController
@superclass UIViewController <NKSplitViewPopoverButtonDelegate>
@abstract
@discussion
*/
@interface TRVNavigationBarViewController : UIViewController <NKSplitViewPopoverButtonDelegate>

@property (nonatomic, retain) UINavigationBar *navigationBar;

@end
