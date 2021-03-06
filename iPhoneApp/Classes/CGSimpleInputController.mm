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

#import "CGSimpleInputController.h"
#import "AppCore.h"
#import "APIFactory.h"
#import "DebugConfig.h"
#import "AppConfig.h"
#include "MiniGL.h"


@implementation CGSimpleInputController


- (id)initWithAppCore:(WVSClientCommon::AppCore	*)appCore camera:(MiniGL::SimpleCamera *)camera
{
    if ((self = [super init]))
	{		
        _appCore = appCore;
		_camera = camera;
		_panningMode = false;
    }
    return self;
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
	_appCore->preUserInput();
	[super touchesBegan:touches withEvent:event];
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event 
{
	[super touchesEnded:touches withEvent:event];
	_appCore->postUserInput();
}

- (void)doubleTap
{
	#if USE_STATIC_POINT_CLOUD
	#else
	_appCore->requestPointCloudForCurrentView();
	#endif
	_panningMode = !_panningMode;
}


- (void)singleTouchMoveDelta:(CGPoint)moveDelta
{
	WVSClientCommon::APIFactory::GetInstance().lock(WVSClientCommon::OPENGL_LOCK);
	
	//NSLog(@"%f_ST_%f_%f\n", WVSClientCommon::APIFactory::GetInstance().getTimeInMS(), moveDelta.x, moveDelta.y);
	
//	if (_panningMode)
//	{
//		_appCore->moveStrafe(-moveDelta.x / 500.0f * WVSClientCommon::OCTREE_SCALE);
//		_appCore->moveUp(moveDelta.y / 500.0f * WVSClientCommon::OCTREE_SCALE);
//	}
//	else
	{
		if ((fabsf(moveDelta.x) > 2.0f) && (fabsf(moveDelta.y) <= 10.0f)) _camera->changeHeading(moveDelta.x / 5.0f);
		if (fabsf(moveDelta.y) > 2.0f) _camera->forward(moveDelta.y / 500.0f * WVSClientCommon::OCTREE_SCALE);
	}
	
	WVSClientCommon::APIFactory::GetInstance().unlock(WVSClientCommon::OPENGL_LOCK);
}


- (void)doubleTouchMoveDelta1:(CGPoint)moveDelta1 delta2:(CGPoint)moveDelta2;
{
	WVSClientCommon::APIFactory::GetInstance().lock(WVSClientCommon::OPENGL_LOCK);

	//NSLog(@"%f_DT_%f_%_%f_%f\n", WVSClientCommon::APIFactory::GetInstance().getTimeInMS(), moveDelta1.x, moveDelta1.y, moveDelta2.x, moveDelta2.y);
	_camera->strafe(-(moveDelta1.x + moveDelta2.x) / 500.0f * WVSClientCommon::OCTREE_SCALE);
	_camera->up((moveDelta1.y + moveDelta2.y) / 500.0f * WVSClientCommon::OCTREE_SCALE);
	
	WVSClientCommon::APIFactory::GetInstance().unlock(WVSClientCommon::OPENGL_LOCK);
}


- (void)tripleTap
{
	//_appCore->getCamera()->reset();
}


@end
