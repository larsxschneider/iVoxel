//
//  CGMainController.h
//  MacApp
//
//  Created by Lars Schneider on 19.05.10.
//  Copyright 2010 Lars Schneider. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <Quartz/Quartz.h>


@class CGOpenGLView;


namespace WVSClientCommon
{
class AppCore;
}


namespace MiniGL
{
class SimpleCamera;
}


@interface CGMainController : NSResponder
{
	IBOutlet CGOpenGLView		*_openGLView;
	IBOutlet IKImageView		*_wvsImageView;
	BOOL						_isAnimating;
	WVSClientCommon::AppCore	*_appCore;
	MiniGL::SimpleCamera		*_camera;
}


@property(nonatomic, readonly) WVSClientCommon::AppCore *appCore;
@property(nonatomic, readonly) MiniGL::SimpleCamera *camera;


@end
