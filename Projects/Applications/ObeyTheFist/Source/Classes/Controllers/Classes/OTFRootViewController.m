
#import "OTFRootViewController.h"
#import "OTFConfiguration.h"

@implementation OTFRootViewController

	-(id) init {
		self = [super initWithNibName:nil bundle:nil];
		if (!self) {
			return nil;
		}
		self.wantsFullScreenLayout = YES;
		return self;
	}

	-(void) loadView {
		[super viewDidLoad];
		self.view  = [EAGLView viewWithFrame:[UIScreen mainScreen].bounds 
								 pixelFormat:kEAGLColorFormatRGB565 
								 depthFormat:0 
						  preserveBackbuffer:NO];
	}

	-(void) viewDidLoad {
		[super viewDidLoad];
	}

	/*!
	@abstract 
	There are 2 ways to support auto-rotation:
	- The OpenGL / cocos2d way
	- Faster, but doesn't rotate the UIKit objects
	- The ViewController way
	- A bit slower, but the UiKit objects are placed in the right place
	*/
	-(BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
		#if GAME_AUTOROTATION == kGameAutorotationNone
		// EAGLView won't be autorotated
		return NO;

		#elif GAME_AUTOROTATION == kGameAutorotationCCDirector
		// EAGLView will be rotated by cocos2d
		// Sample: Autorotate only in landscape mode
		if (interfaceOrientation == UIInterfaceOrientationLandscapeLeft) {
			[[CCDirector sharedDirector] setDeviceOrientation:kCCDeviceOrientationLandscapeRight];
		}
		else if (interfaceOrientation == UIInterfaceOrientationLandscapeRight)  {
			[[CCDirector sharedDirector] setDeviceOrientation:kCCDeviceOrientationLandscapeLeft];
		}

		return NO;

		#elif GAME_AUTOROTATION == kGameAutorotationUIViewController
		// EAGLView will be rotated by the UIViewController
		// Sample: Autorotate only in landscpe mode return YES for the supported orientations
		if ((interfaceOrientation == UIInterfaceOrientationLandscapeLeft) ||
			(interfaceOrientation == UIInterfaceOrientationLandscapeRight) ) {
			return YES;
		}
		// Unsupported orientations: UIInterfaceOrientationPortrait, UIInterfaceOrientationPortraitUpsideDown
		return NO;
		#else
			#error Unknown value in GAME_AUTOROTATION
		#endif
		return NO;
	}

	/*!
	@abstract  This callback only will be called when GAME_AUTOROTATION == kGameAutorotationUIViewController
	*/
	#if GAME_AUTOROTATION == kGameAutorotationUIViewController
	-(void) willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration {
		//
		// Assuming that the main window has the size of the screen
		// BUG: This won't work if the EAGLView is not fullscreen
		///
		CGRect screenRect = [[UIScreen mainScreen] bounds];
		CGRect rect;

		if ((toInterfaceOrientation == UIInterfaceOrientationPortrait) || (toInterfaceOrientation == UIInterfaceOrientationPortraitUpsideDown)) {
			rect = screenRect;
		}
		else if ((toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft) || (toInterfaceOrientation == UIInterfaceOrientationLandscapeRight) )   {
			rect.size = CGSizeMake( screenRect.size.height, screenRect.size.width );
		}

		CCDirector *director = [CCDirector sharedDirector];
		EAGLView *directorGLView = [director openGLView];
		float contentScaleFactor = [director contentScaleFactor];
		if (contentScaleFactor != 1) {
			rect.size.width *= contentScaleFactor;
			rect.size.height *= contentScaleFactor;
		}
		directorGLView.frame = rect;
	}
	#endif

	#pragma mark -
	
	-(EAGLView *) glView {
		return (EAGLView *)self.view;
	}

	#pragma mark -

	-(void) didReceiveMemoryWarning {
		[super didReceiveMemoryWarning];
	}

	-(void) viewDidUnload {
		[super viewDidUnload];
	}

	-(void) dealloc {
		[super dealloc];
	}

@end
