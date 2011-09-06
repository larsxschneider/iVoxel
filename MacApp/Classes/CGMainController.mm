//
//  CGMainController.m
//  MacApp
//
//  Created by Lars Schneider on 19.05.10.
//  Copyright 2010 Lars Schneider. All rights reserved.
//

#import "CGMainController.h"
#import "CGOpenGLView.h"
#import "AppCore.h"
#import "PerformanceTests.h"
#import "DebugConfig.h"
#import "AppConfig.h"
#import "APIFactory.h"
#include "MiniGL.h"


@implementation CGMainController


@synthesize appCore = _appCore,
			camera = _camera;


#if RUN_PERFORMANCE_TEST
- (void)performanceTest
{
	WVSClientCommon::Tests::MemoryPoolTest();
}
#endif


- (void)awakeFromNib
{
#if RUN_PERFORMANCE_TEST
	[self performSelector:@selector(performanceTest) withObject:nil afterDelay:1.0];
#else
	[_openGLView setupSharedContext:nil];
		
	// Allocate camera and scene
	_camera = new MiniGL::SimpleCamera(WVSClientCommon::CAMERA_START_POSITION, WVSClientCommon::CAMERA_START_HEADING_RADIANS);
	_appCore = new WVSClientCommon::AppCore(_camera);
#endif
}


- (void)dealloc
{
	delete _appCore;
	delete _camera;
	[super dealloc];
}


- (void)keyUp:(NSEvent *)theEvent
{
	_appCore->postUserInput();
}

- (void)keyDown:(NSEvent *)theEvent
{
	if (!theEvent.isARepeat) _appCore->preUserInput();
	
	static const float factor = 1.1f;
	
    unichar c = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	
	if ( c == 'r')_appCore->setRenderingRequired();

	WVSClientCommon::APIFactory::GetInstance().lock(WVSClientCommon::OPENGL_LOCK);
		
    switch (c)
	{
		case 'e':
		     _camera->forward(1.0f * factor);
            break;
		case 'E':
            _camera->forward(10.0f * factor);
            break;
			
		case 'd':
		case 'D':
            _camera->forward(-1.0f * factor);
            break;
	
		case 'q':
		case 'Q':
            _camera->changePitch(-1.0f);
            break;
			
		case 'a':
		case 'A':
            _camera->changePitch(1.0f);
            break;
			
		case 's':
		case 'S':
            _camera->strafe(-1.0f * factor);
            break;

		case 'f':
		case 'F':
            _camera->strafe(1.0f * factor);
            break;
			
		case NSUpArrowFunctionKey: 
			_camera->up(1.0f * factor);
			break;
			
		case NSDownArrowFunctionKey:
			_camera->up(-1.0f * factor);
			break;
			
		case NSRightArrowFunctionKey: 
			_camera->changeHeading(-10.0f);
			break;
			
		case NSLeftArrowFunctionKey:
			_camera->changeHeading(10.0f);
			break;
			
        default:
            break;
    }
	
	WVSClientCommon::APIFactory::GetInstance().unlock(WVSClientCommon::OPENGL_LOCK);
}


@end
