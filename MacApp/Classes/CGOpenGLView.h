//
//  CGOpenGLView.h
//  MacApp
//
//  Created by Lars Schneider on 27.05.10.
//  Copyright 2010 Lars Schneider. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>


@class CGMainController;


@interface CGOpenGLView : NSView
{
	IBOutlet CGMainController	*_controller;
	NSOpenGLContext				*_openGLContext;
	NSOpenGLPixelFormat			*_pixelFormat;
	BOOL						_viewSetupComplete;
}

- (void)setupSharedContext:(NSOpenGLContext*)context;

- (NSOpenGLContext*)openGLContext;

@end
