#ifndef _MINIGL_GL_ERROR_H_
#define _MINIGL_GL_ERROR_H_


namespace MiniGL
{
		
#ifdef DEBUG
void checkGL(void);
#define CHECK_GL MiniGL::checkGL()
#else
#define CHECK_GL
#endif
 
}


#endif // _MINIGL_GL_ERROR_H_