//
//  iPhoneAppAppDelegate.m
//  iPhoneApp
//
//  Created by Lars Schneider on 19.05.10.
//  Copyright Lars Schneider 2010. All rights reserved.
//

#import "AppDelegate.h"
#import "EAGLView.h"
#import "AppCore.h"
#import "CGSimpleInputController.h"
#import "PerformanceTests.h"
#import "DebugConfig.h"
#import "CGCityInputController.h"
#import "MiniGL.h"
#import "AppConfig.h"
#import <mach/mach.h>


@implementation AppDelegate


@synthesize appCore = _appCore,
			camera = _camera;


#if RUN_PERFORMANCE_TEST
- (void)performanceTest
{
	WVSClientCommon::Tests::NEONMemCpyTest();
}
#endif


- (BOOL)application:(UIApplication *)application
	didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
#if RUN_PERFORMANCE_TEST
	[self performSelector:@selector(performanceTest) withObject:nil afterDelay:1.0];
#else

	_window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

#if !USE_STATIC_POINT_CLOUD
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    BOOL access = [prefs boolForKey:@"wvsAccess"];
	
	if (!access)
	{
		_authVC = [[CGAuthViewController alloc] initWithNibName:@"CGAuthView" bundle:nil];
		[_window addSubview:_authVC.view];
	}
	else
#endif
	{
		[self startAppCore];
	}

	[_window makeKeyAndVisible];
	
#endif	
    return YES;
}


- (void)dealloc
{
    [_window release];
	[_glView release];
	if (_appCore != NULL) delete _appCore;
	if (_camera != NULL) delete _camera;

    [super dealloc];
}


- (void)startAppCore
{
	_glView = [[EAGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds]]; 
	
	_camera = new MiniGL::SimpleCamera(WVSClientCommon::CAMERA_START_POSITION, WVSClientCommon::CAMERA_START_HEADING_RADIANS);
	_appCore = new WVSClientCommon::AppCore(_camera);
	
	CGSimpleInputController *ic = [[CGSimpleInputController alloc] initWithAppCore:_appCore camera:_camera];
	//CGCityInputController *ic = [[CGCityInputController alloc] initWithAppCore:_appCore camera:_camera];
	
	[_glView addInputController:ic];
	[ic release];
	
	[_window addSubview:_glView];
	
	[_authVC release];
}


- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
	struct task_basic_info info;
	mach_msg_type_number_t size = sizeof(info);
	kern_return_t kerr = task_info(mach_task_self(),
								 TASK_BASIC_INFO,
								 (task_info_t)&info,
								 &size);
	if( kerr == KERN_SUCCESS )
	{
		NSLog(@"Memory used: %u", info.resident_size); //in bytes
	}
	else
	{
	NSLog(@"Error: %s", mach_error_string(kerr));
	}
}

@end
