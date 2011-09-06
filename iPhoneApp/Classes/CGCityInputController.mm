//
//  CGCityInputController.m
//  iPhoneApp
//
//  Created by Lars Schneider on 27.10.10.
//  Copyright 2010 Lars Schneider. All rights reserved.
//

#import "CGCityInputController.h"
#import "AppCore.h"
#import "APIFactory.h"
#import "DebugConfig.h"
#import "AppConfig.h"

#define kFilteringFactor   0.1

@implementation CGCityInputController


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
	_inputMode = !_inputMode;
}


- (void)singleTouchMoveDelta:(CGPoint)moveDelta
{
	WVSClientCommon::APIFactory::GetInstance().lock(WVSClientCommon::OPENGL_LOCK);

	if (_inputMode == 0)
	{
		_camera->forward(moveDelta.y / 500.0f * WVSClientCommon::OCTREE_SCALE);
		_camera->strafe(-moveDelta.x / 500.0f * WVSClientCommon::OCTREE_SCALE);
	}
	else
	{
		if ((fabsf(moveDelta.x) > 2.0f) && (fabsf(moveDelta.y) <= 10.0f)) _camera->changeHeading(moveDelta.x / 5.0f);
		if ((fabsf(moveDelta.y) > 2.0f) && (fabsf(moveDelta.x) <= 10.0f)) _camera->changePitch(moveDelta.y / 5.0f);
	}
	
	WVSClientCommon::APIFactory::GetInstance().unlock(WVSClientCommon::OPENGL_LOCK);
}


- (void)doubleTouchMoveDelta1:(CGPoint)moveDelta1 delta2:(CGPoint)moveDelta2;
{
	WVSClientCommon::APIFactory::GetInstance().lock(WVSClientCommon::OPENGL_LOCK);
	
	//NSLog(@"%f_DT_%f_%_%f_%f\n", WVSClientCommon::APIFactory::GetInstance().getTimeInMS(), moveDelta1.x, moveDelta1.y, moveDelta2.x, moveDelta2.y);
	//_appCore->moveStrafe(-(moveDelta1.x + moveDelta2.x) / 500.0f * WVSClientCommon::OCTREE_SCALE);
	_camera->up((moveDelta1.y + moveDelta2.y) / 500.0f * WVSClientCommon::OCTREE_SCALE);
	
	WVSClientCommon::APIFactory::GetInstance().unlock(WVSClientCommon::OPENGL_LOCK);
}


- (void)tripleTap
{
	//_appCore->getCamera()->reset();
}


@end
