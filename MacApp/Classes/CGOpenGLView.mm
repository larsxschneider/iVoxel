//
//  CGOpenGLView.m
//  MacApp
//
//  Created by Lars Schneider on 27.05.10.
//  Copyright 2010 Lars Schneider. All rights reserved.
//

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
