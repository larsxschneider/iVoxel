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
