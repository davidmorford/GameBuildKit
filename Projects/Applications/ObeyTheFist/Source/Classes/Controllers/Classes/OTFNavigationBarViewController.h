
/*!
@project	ObeyTheFist
@header     OTFNavigationBarViewController.h
@created	10/9/10
*/

#import <UIKit/UIKit.h>
#import <NavigatorKit/NavigatorKit.h>

/*!
@class ObeyTheFistNavigationBarViewController
@superclass UIViewController <NKSplitViewPopoverButtonDelegate>
@abstract
@discussion
*/
@interface OTFNavigationBarViewController : UIViewController <NKSplitViewPopoverButtonDelegate>

@property (nonatomic, retain) UINavigationBar *navigationBar;

@end
