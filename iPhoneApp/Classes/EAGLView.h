//
//  EAGLView.h
//  iPhoneApp
//
//  Created by Lars Schneider on 19.05.10.
//  Copyright Lars Schneider 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>


@class GLViewInputController;


// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView
{    
@private
	EAGLContext *_context;

//    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
//    GLuint		_framebuffer[3];
//	GLuint		_textures[2];
	GLuint		_depthBuffer;
	GLuint		_colorBuffer, colorRenderbuffer, defaultFramebuffer;
 
	NSMutableArray              *_glViewInputControllerArray;
}

- (void)addInputController:(GLViewInputController*)glViewInputController;
- (void)removeInputController:(GLViewInputController*)glViewInputController;

@end
