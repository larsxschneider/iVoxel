//
//  iPhoneAppAppDelegate.h
//  iPhoneApp
//
//  Created by Lars Schneider on 19.05.10.
//  Copyright Lars Schneider 2010. All rights reserved.
//

#import <UIKit/UIKit.h>


@class EAGLView;
@class CGAuthViewController;


namespace WVSClientCommon
{
class AppCore;
}


namespace MiniGL
{
class SimpleCamera;
}


@interface AppDelegate : NSObject <UIApplicationDelegate>
{
    UIWindow					*_window;
	CGAuthViewController		*_authVC;
	EAGLView					*_glView;
	WVSClientCommon::AppCore	*_appCore;
	MiniGL::SimpleCamera		*_camera;
}


@property(nonatomic, readonly) WVSClientCommon::AppCore *appCore;
@property(nonatomic, readonly) MiniGL::SimpleCamera *camera;


- (void)startAppCore;


@end

