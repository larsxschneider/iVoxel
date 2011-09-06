/*
 *  PlatformOpenGL.h
 *
 *  Created by Lars Schneider on 26.05.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
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
