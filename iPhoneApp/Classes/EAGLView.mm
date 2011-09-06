//
//  EAGLView.m
//  iPhoneApp
//
//  Created by Lars Schneider on 19.05.10.
//  Copyright Lars Schneider 2010. All rights reserved.
//

#import "EAGLView.h"
#import "AppDelegate.h"
#import "APIFactory.h"
#import "AppCore.h"
#import "GLViewInputController.h"
#import "DebugConfig.h"
#import "AppConfig.h"
#import "Timer.h"

@implementation EAGLView


// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}


- (id)initWithFrame:(CGRect)frame
{    
    if ((self = [super initWithFrame:frame]))
	{
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO],
										kEAGLDrawablePropertyRetainedBacking,
										kEAGLColorFormatRGB565,
										kEAGLDrawablePropertyColorFormat,
										nil];

        _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

        if (!_context || ![EAGLContext setCurrentContext:_context])
        {
            [_context release];
            return nil;
        }

		glGenFramebuffers(1, &defaultFramebuffer);
		glGenRenderbuffers(1, &colorRenderbuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

		self.multipleTouchEnabled = YES;
        _glViewInputControllerArray = [[NSMutableArray alloc] init];
    }

    return self;
}


- (void)dealloc
{
	[_glViewInputControllerArray release];
	_glViewInputControllerArray = nil;
	
    // Tear down GL
    if (defaultFramebuffer)
    {
        glDeleteFramebuffers(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }

    if (colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
	
    // Tear down context
    if ([EAGLContext currentContext] == _context)
        [EAGLContext setCurrentContext:nil];

    [_context release];
    _context = nil;
	
    [super dealloc];
}


- (void)startMainLoop
{
	[EAGLContext setCurrentContext:_context];
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
	[_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
	
	((AppDelegate *)[[UIApplication sharedApplication]
		delegate]).appCore->mainLoop(	_context, 
										&defaultFramebuffer,
										&colorRenderbuffer,
										[UIScreen mainScreen].bounds.size.width,
										[UIScreen mainScreen].bounds.size.height);
}


- (void)layoutSubviews
{
#if !RUN_PERFORMANCE_TEST
	[self performSelectorOnMainThread:@selector(startMainLoop) withObject:nil waitUntilDone:NO];
#endif
}


- (void)addInputController:(GLViewInputController*)glViewInputController
{
    [_glViewInputControllerArray addObject:glViewInputController];
}


- (void)removeInputController:(GLViewInputController*)glViewInputController
{
    [_glViewInputControllerArray removeObject:glViewInputController];
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
    for (GLViewInputController* inputController in _glViewInputControllerArray) 
    {
		if (inputController.isActive == YES)
		{
			[inputController touchesBegan:touches withEvent:event];
		}
    }
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (GLViewInputController* inputController in _glViewInputControllerArray) 
    {
		if (inputController.isActive == YES)
		{
			[inputController touchesMoved:touches withEvent:event];
		}
    }
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (GLViewInputController* inputController in _glViewInputControllerArray) 
    {
		if (inputController.isActive == YES)
		{
			[inputController touchesEnded:touches withEvent:event];
		}
    }	
}


@end
