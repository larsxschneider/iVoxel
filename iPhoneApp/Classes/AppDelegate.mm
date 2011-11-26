/*
 *  Copyright (c) 2011, Lars Schneider
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer.
 *  Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

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
