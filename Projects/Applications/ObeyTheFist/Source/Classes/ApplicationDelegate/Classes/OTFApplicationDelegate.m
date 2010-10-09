
#import "OTFApplicationDelegate.h"
#import "OTFRootViewController.h"
#import "OTFConfiguration.h"
#import "OTFUserDefaults.h"

#import "OTFSceneLayer.h"

@interface OTFApplicationDelegate ()
@end

#pragma mark -

@implementation OTFApplicationDelegate

	@synthesize window;
	@synthesize applicationDocumentsDirectoryPath;

	+(void) initialize {
		NSDictionary *defaults = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"UserDefaults" ofType:@"plist"]];
		[[NSUserDefaults standardUserDefaults] registerDefaults:defaults];
	}

	#pragma mark Shared Delegate

	+(OTFApplicationDelegate *) sharedApplicationDelegate {
		return (OTFApplicationDelegate *)[UIApplication sharedApplication].delegate;
	}

	#pragma mark <UIApplicationDelegate>

	-(BOOL) application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
		NKNavigator *sharedNavigator		= [NKNavigator navigator];
		sharedNavigator.defaultURLScheme	= @"obeythefist";
		sharedNavigator.delegate			= self;
		sharedNavigator.wantsNavigationControllerForRoot = NO;
		NKNavigatorMap *navigationMap		= sharedNavigator.navigationMap;
		
		[navigationMap from:@"obeythefist://root" toSharedViewController:[OTFRootViewController class]];
		OTFRootViewController *rootViewController = [sharedNavigator viewControllerForURL:@"obeythefist://root"];

		[CCDirector setDirectorType:kCCDirectorTypeDisplayLink];
		CCDirector *director = [CCDirector sharedDirector];

		// attach the openglView to the director
		[director setOpenGLView:rootViewController.glView];

		// To enable Hi-Red mode (iPhone4)
		[director setContentScaleFactor:2];

		// If the rotation is going to be controlled by a UIViewController then the device orientation should be "Portrait".
		#if GAME_AUTOROTATION == kGameAutorotationUIViewController
			[director setDeviceOrientation:kCCDeviceOrientationPortrait];
		#else
			[director setDeviceOrientation:kCCDeviceOrientationLandscapeLeft];
		#endif

		[director setAnimationInterval:1.0/60];
		[director setDisplayFPS:YES];

		// Default texture format for PNG/BMP/TIFF/JPEG/GIF images
		// It can be RGBA8888, RGBA4444, RGB5_A1, RGB565 You can change anytime.
		[CCTexture2D setDefaultAlphaPixelFormat:kCCTexture2DPixelFormat_RGBA8888];

		sharedNavigator.window.rootViewController = rootViewController;
		
		[sharedNavigator.window makeKeyAndVisible];
		
		[[CCDirector sharedDirector] runWithScene:[OTFSceneLayer scene]];

		return TRUE;
	}

	-(void) applicationWillResignActive:(UIApplication *)application {
		[[CCDirector sharedDirector] pause];
	}

	-(void) applicationDidBecomeActive:(UIApplication *)application {
		[[CCDirector sharedDirector] resume];
	}

	-(void) applicationDidReceiveMemoryWarning:(UIApplication *)application {
		[[CCDirector sharedDirector] purgeCachedData];
	}

	-(void) applicationDidEnterBackground:(UIApplication *)application {
		[[CCDirector sharedDirector] stopAnimation];
	}

	-(void) applicationWillEnterForeground:(UIApplication *)application {
		[[CCDirector sharedDirector] startAnimation];
	}

	-(void) applicationWillTerminate:(UIApplication *)application {
//		CCDirector *director = [CCDirector sharedDirector];
//		[[director openGLView] removeFromSuperview];
//		[viewController release];
//		[window release];
//		[director end];
	}

	-(void) applicationSignificantTimeChange:(UIApplication *)application {
		[[CCDirector sharedDirector] setNextDeltaTimeZero:YES];
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


	#pragma mark <NKNavigatorDelegate>

	-(void) navigatorDidEnterBackground:(NKNavigator *)navigator {

	}

	-(void) navigatorWillEnterForeground:(NKNavigator *)navigator {

	}

	-(void) navigator:(NKNavigator *)navigator didLoadController:(UIViewController *)controller {

	}

	-(void) navigator:(NKNavigator *)navigator didUnloadViewController:(UIViewController *)controller {

	}

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
		[[CCDirector sharedDirector] release];
		[window release];
		[super dealloc];
	}

@end
