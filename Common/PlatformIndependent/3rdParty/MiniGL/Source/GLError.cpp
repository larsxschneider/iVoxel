#include "GLError.h"
#include "PlatformOpenGL.h"
#include <stdio.h>

namespace MiniGL
{
		
	
void checkGL(void)
{   
    GLenum err;
    if (err = glGetError())
    {
        switch (err)
        {
            case GL_INVALID_ENUM:
                fprintf(stderr, "OPENGL ERROR: GL_INVALID_ENUM");
                //BREAKPOINT;
                break;
 
            case GL_INVALID_VALUE:
                fprintf(stderr, "OPENGL ERROR: GL_INVALID_VALUE");
                //BREAKPOINT;
                break;
 
            case GL_INVALID_OPERATION:
                fprintf(stderr, "OPENGL ERROR: GL_INVALID_OPERATION");
                //BREAKPOINT;
                break;
 
            case GL_OUT_OF_MEMORY:
                fprintf(stderr,"OPENGL ERROR: GL_OUT_OF_MEMORY");
                //BREAKPOINT;
                break;              
 
            default:
                fprintf(stderr,"OPENGL ERROR: 0x%x", err);
                //BREAKPOINT;
                break;
        }
 
    }
}


}