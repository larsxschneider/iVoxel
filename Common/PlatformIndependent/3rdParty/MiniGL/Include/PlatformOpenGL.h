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

#ifndef _PLATFORM_OPENGL_H_
#define _PLATFORM_OPENGL_H_

#ifdef __APPLE__
	#include "TargetConditionals.h"
   
	#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
	
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
	
	#define BREAKPOINT __asm__ volatile ("bkpt 1")
	#define OPENGL_ES
	#endif
	
	#if (!TARGET_IPHONE_SIMULATOR && !TARGET_OS_IPHONE && TARGET_OS_MAC)
	
	#include <OpenGL/glu.h>
	
	#define BREAKPOINT __asm__ volatile ("int3");
	#define OPENGL
	#endif
#endif

#if _WIN32
	#include <windows.h>
	#include <GL/glew.h>
#endif // _WIN32

#endif // _PLATFORM_OPENGL_H_
