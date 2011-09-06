//
//  CGSimpleInputController.h
//  iPhoneApp
//
//  Created by Lars Schneider on 10.06.10.
//  Copyright 2010 Lars Schneider. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "GLViewInputController.h"


namespace WVSClientCommon
{
class AppCore;
}


namespace MiniGL
{
class SimpleCamera;
}


@interface CGSimpleInputController : GLViewInputController
{
	WVSClientCommon::AppCore	*_appCore;
	MiniGL::SimpleCamera		*_camera;
	bool						_panningMode;
}


- (id)initWithAppCore:(WVSClientCommon::AppCore	*)appCore camera:(MiniGL::SimpleCamera *)camera;


@end
