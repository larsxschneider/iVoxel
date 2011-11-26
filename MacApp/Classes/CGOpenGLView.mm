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

#import "CGOpenGLView.h"
#import "CGMainController.h"
#import "AppCore.h"
#import "APIFactory.h"
#import "DebugConfig.h"
#import <OpenGL/CGLRenderers.h>
#import "AppConfig.h"


@interface CGOpenGLView (PrivateMethods)
- (void)setupDisplayLink;
- (void)drawView;
@end


@implementation CGOpenGLView


- (NSOpenGLContext*)openGLContext
{
	return _openGLContext;
}


- (void)setupSharedContext:(NSOpenGLContext*)context
{
    NSOpenGLPixelFormatAttribute attribs[] =
    {
#if OSX_SOFTWARE_RENDERER
		kCGLPFARendererID,
		kCGLRendererGenericFloatID,
#else
//		kCGLPFAAccelerated,
//		kCGLPFANoRecovery,
#endif
		kCGLPFADoubleBuffer,
		kCGLPFAColorSize, 24,
		kCGLPFADepthSize, 16,
		0
    };
	
    _pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
	
    if (!_pixelFormat)
		NSLog(@"No OpenGL pixel format");
	
	// NSOpenGLView does not handle context sharing, so we draw to a custom NSView instead
	_openGLContext = [[NSOpenGLContext alloc] initWithFormat:_pixelFormat shareContext:context];
	
	[[self openGLContext] makeCurrentContext];

}

- (void)lockFocus
{
	[super lockFocus];
	if ([[self openGLContext] view] != self)
		[[self openGLContext] setView:self];
}

- (void)startMainLoop
{
	_controller.appCore->mainLoop(_openGLContext, NULL, NULL, [self bounds].size.width, [self bounds].size.height);
}

- (void)reshape
{
#if !RUN_PERFORMANCE_TEST

	if (_viewSetupComplete) return;

	// This method will be called on the main thread when resizing, but we may be drawing on a secondary thread through the display link
	// Add a mutex around to avoid the threads accessing the context simultaneously
	CGLLockContext((_CGLContextObject*)[_openGLContext CGLContextObj]);
	
	// Make sure we draw to the right context
	[[self openGLContext] makeCurrentContext];
	
	NSUInteger width = [self bounds].size.width;
	NSUInteger height =	[self bounds].size.height;
	
	// WVS only accepts even widths and heights (source: Benjamin Hagedorn)
	if (width % 2 != 0) width--;
	if (height % 2 != 0) height--;
	
	WVSClientCommon::APIFactory::GetInstance().lock(WVSClientCommon::OPENGL_LOCK);
	glViewport(0, 0, width, height);
	_controller.camera->setFrustum(width, height, 
						WVSClientCommon::CAMERA_FIELD_OF_VIEW_IN_DEGREE,
						WVSClientCommon::CAMERA_NEAR_PLANE,
						WVSClientCommon::CAMERA_FAR_PLANE);
	WVSClientCommon::APIFactory::GetInstance().unlock(WVSClientCommon::OPENGL_LOCK);
						
	// Delegate to the scene object to update for a change in the view size
	_controller.appCore->updateScreenSizeRelatedConstants();
	
	[[self openGLContext] update];
	
	CGLUnlockContext((_CGLContextObject*)[_openGLContext CGLContextObj]);
	
	_viewSetupComplete = YES;
	
	[self performSelectorOnMainThread:@selector(startMainLoop) withObject:nil waitUntilDone:NO];
#endif
}

- (BOOL)acceptsFirstResponder
{
    // We want this view to be able to receive key events
    return YES;
}


- (void)keyDown:(NSEvent *)theEvent
{
    // Delegate to the controller object for handling key events
    [_controller keyDown:theEvent];
}


- (void)keyUp:(NSEvent *)theEvent
{
    // Delegate to the controller object for handling key events
    [_controller keyUp:theEvent];
}


- (void)mouseDown:(NSEvent *)theEvent
{
    // Delegate to the controller object for handling mouse events
    [_controller mouseDown:theEvent];
}

@end
