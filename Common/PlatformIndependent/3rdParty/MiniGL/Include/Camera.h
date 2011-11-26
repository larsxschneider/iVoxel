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