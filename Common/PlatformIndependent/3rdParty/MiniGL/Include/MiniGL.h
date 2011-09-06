/*
 *  OpenGL.h
 *
 *  Created by Lars Schneider on 21.05.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */
 
#ifndef _MINIGL_OPENGL_H_
#define _MINIGL_OPENGL_H_

#include <cmath>
#include "PlatformOpenGL.h"
#include "Macros.h"
#include <stdint.h>
#include "GLError.h"

// Camera
#include "Camera.h"
#include "SimpleCamera.h"
#include "ViewFrustum.h"

// Math
#include "Matrix.h"

// Scene Objects
#include "Camera.h"
#include "CoordinateSystem.h"
#include "Plane.h"
#include "FrustumSceneObject.h"
#include "Skybox.h"

// Resources
#include "ResourceShader.h"
#include "ResourceImage.h"

#endif // _MINIGL_OPENGL_H_