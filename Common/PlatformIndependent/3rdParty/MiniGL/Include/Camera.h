/*
 *  Camera.h
 *  MacApp
 *
 *  Created by Lars Schneider on 26.05.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#ifndef _MINIGL_CAMERA_H_
#define _MINIGL_CAMERA_H_

#include "Vector.h"
#include "Matrix.h"
#include "Macros.h"

namespace MiniGL
{
		
		
class Camera
{

public:
	
	Camera() {};
	virtual ~Camera() {};
	
	virtual void getFrustum(MATRIX &frustum) const = 0;
	virtual void getLookAt(MATRIX &look) = 0;
	virtual void getPosition(VECTOR3 &pos) const = 0;
	virtual void getDirection(VECTOR3 &dir) const = 0;
	virtual void getTo(VECTOR3 &to) const = 0;
	virtual void getUp(VECTOR3 &up) const = 0;
	virtual float_t getFieldOfViewInDegree() const = 0;
	virtual float_t getFieldOfViewInRad() const = 0;	
	virtual float_t getZNear() const = 0;
	virtual float_t getZFar() const = 0;
	virtual float_t getAspectRatio() const = 0;
	virtual uint16_t getScreenWidth() const = 0;
	virtual uint16_t getScreenHeight() const = 0;
};


} // namespace MiniGL


#endif // _MINIGL_CAMERA_H_