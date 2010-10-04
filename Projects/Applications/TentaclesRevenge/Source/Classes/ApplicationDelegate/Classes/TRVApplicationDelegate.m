
#import "TRVApplicationDelegate.h"
#import "TRVMasterViewController.h"
#import "TRVDetailViewController.h"
#import "TRVNavigationBarViewController.h"
#import "TRVSettingsViewController.h"
#import "TRVUserDefaults.h"

@interface TRVApplicationDelegate ()
@end

#pragma mark -

@implementation TRVApplicationDelegate

@synthesize window;
@synthesize splitViewController;
@synthesize applicationDocumentsDirectoryPath;

+(void) initialize {
	NSDictionary *defaults = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"UserDefaults" ofType:@"plist"]];
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaults];
}

#pragma mark Shared Delegate

+(TRVApplicationDelegate *) sharedApplicationDelegate {
	return (TRVApplicationDelegate *)[UIApplication sharedApplication].delegate;
}

#pragma mark <UIApplicationDelegate>

-(BOOL) application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	if (NKUIDeviceUserIntefaceIdiom() == UIUserInterfaceIdiomPad) {
		
		//
		// Split View Navigator
		//
		NKSplitViewNavigator *sharedNavigator	 = [NKSplitViewNavigator splitViewNavigator];
		sharedNavigator.delegate				 = self;
		sharedNavigator.defaultURLScheme		 = @"tentaclesrevenge";
		sharedNavigator.masterPopoverButtonTitle = @"Master";

		NKNavigatorMap *sharedNavigationMap = sharedNavigator.navigationMap;
		[sharedNavigationMap from:@"tentaclesrevenge://splitView"	toObject:sharedNavigator.rootViewController];
		[sharedNavigationMap from:@"tentaclesrevenge://settings"	toModalViewController:[TRVSettingsViewController class] presentationStyle:UIModalPresentationFormSheet];
		
		//
		// Master Navigator
		//
		NKNavigatorMap *masterNavigationMap = sharedNavigator.masterNavigator.navigationMap;
		sharedNavigator.masterNavigator.delegate = self;
		[masterNavigationMap from:@"tentaclesrevenge://master" toEmptyHistoryViewController:[TRVMasterViewController class]];
		
		//
		// Detail Navigator
		//
		sharedNavigator.detailNavigator.delegate = self;
		NKNavigatorMap *detailNavigationMap = sharedNavigator.detailNavigator.navigationMap;
		
		[detailNavigationMap from:@"tentaclesrevenge://toolbar" toEmptyHistoryViewController:[TRVDetailViewController class]];
		[detailNavigationMap from:@"tentaclesrevenge://navigationbar" toEmptyHistoryViewController:[TRVNavigationBarViewController class]];
		
		[sharedNavigator setViewControllersWithNavigationURLs:[NSArray arrayWithObjects:@"tentaclesrevenge://master", @"tentaclesrevenge://toolbar", nil]];
	}
	else {
		NKNavigator *sharedNavigator		= [NKNavigator navigator];
		sharedNavigator.defaultURLScheme	= @"tentaclesrevenge";
		sharedNavigator.delegate			= self;
		NKNavigatorMap *navigationMap		= sharedNavigator.navigationMap;

		[navigationMap from:@"tentaclesrevenge://master"		toSharedViewController:[TRVMasterViewController class]];
		[navigationMap from:@"tentaclesrevenge://toolbar"		toViewController:[TRVDetailViewController class]];
		[navigationMap from:@"tentaclesrevenge://navigationbar" toViewController:[TRVNavigationBarViewController class]];
		[navigationMap from:@"tentaclesrevenge://settings"		toModalViewController:[TRVSettingsViewController class]];
		[navigationMap from:@"tentaclesrevenge://navigation"	toObject:sharedNavigator.rootViewController];
		[sharedNavigator openNavigatorAction:[NKNavigatorAction actionWithNavigatorURLPath:@"tentaclesrevenge://master"]];
	}
	return TRUE;
}

-(BOOL) application:(UIApplication *)application handleOpenURL:(NSURL *)aURL {
	NKNavigatorAction *action = [[NKNavigatorAction alloc] initWithNavigatorURLPath:[aURL absoluteString]];
	if (NKUIDeviceUserIntefaceIdiom() == UIUserInterfaceIdiomPad) {
		NKNavigator *nv = [[NKSplitViewNavigator splitViewNavigator] navigatorForURLPath:[aURL absoluteString]];
		if (nv == [NKSplitViewNavigator splitViewNavigator].detailNavigator) {
			[NKSplitViewNavigator splitViewNavigator].splitViewController.detailViewController = [nv openNavigatorAction:action].navigationController;
		}
		else {
			[nv openNavigatorAction:action];
		}
	}
	else {
		action.animated = TRUE;
		[[NKNavigator navigator] openNavigatorAction:action];
	}
	[action release];
	return TRUE;
}

-(void) applicationDidEnterBackground:(UIApplication *)application {
	[[NSUserDefaults standardUserDefaults] setObject:[NSDate date] forKey:@"TRVLastActiveInForegroundDateKey"];
	[[NSUserDefaults standardUserDefaults] synchronize];
}

-(void) applicationWillEnterForeground:(UIApplication *)application {
	
}

-(void) applicationWillResignActive:(UIApplication *)application {
	
}

-(void) applicationDidBecomeActive:(UIApplication *)application {
	
}

-(void) applicationWillTerminate:(UIApplication *)application {
	
}

-(void) applicationDidReceiveMemoryWarning:(UIApplication *)application {
	
}


#pragma mark <NKNavigatorDelegate>

-(void) navigatorDidEnterBackground:(NKNavigator *)navigator {

}

-(void) navigatorWillEnterForeground:(NKNavigator *)navigator {

}

#pragma mark -

-(void) navigator:(NKNavigator *)navigator didLoadController:(UIViewController *)controller {

}

-(void) navigator:(NKNavigator *)navigator didUnloadViewController:(UIViewController *)controller {

}

#pragma mark -

-(void) navigator:(NKNavigator *)navigator willOpenURL:(NSURL *)aURL inViewController:(UIViewController *)controller {

}

-(BOOL) navigator:(NKNavigator *)navigator shouldOpenURL:(NSURL *)aURL {
	return TRUE;
}

-(BOOL) navigator:(NKNavigator *)navigator shouldOpenURL:(NSURL *)URL withQuery:(NSDictionary *)query {
	return TRUE;
}

-(NSURL *) navigator:(NKNavigator *)navigator URLToOpen:(NSURL *)aURL  {
	return aURL;
}


#pragma mark -

-(NSString *) applicationDocumentsDirectoryPath {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *basePath	= ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
	return basePath;
}


#pragma mark -

-(void) dealloc {
	[splitViewController release];
	[window release];
	[super dealloc];
}

@end
