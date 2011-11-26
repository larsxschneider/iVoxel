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

#ifndef _MINIGL_SIMPLE_CAMERA_H_
#define _MINIGL_SIMPLE_CAMERA_H_

#include "MiniGL.h"
#include "Quaternion.h"
#include "Camera.h"


namespace MiniGL
{
		
		
class SimpleCamera : public Camera
{
	const float_t	_startHeadingRadians;
	const float_t	_maxPitchRate;
	const float_t	_maxHeadingRate;
	float_t			_maxForwardVelocity;
	float_t			_headingRadians;
	float_t			_pitchRadians;
	float_t			_forwardVelocity;
	float_t			_fovInRad;
	float_t			_zNear;
	float_t			_zFar;
	float_t			_aspectRatio;
	float_t			_frustumSurroudingSphereRadius;
	uint16_t		_screenWidth;
	uint16_t		_screenHeight;
	QUATERNION		_qHeading;
	QUATERNION		_qPitch;
	Vec3			_startPosition;
	Vec3			_position;
	Vec3			_direction;
	Vec3			_up;
	MATRIX			_projectionMatrix;
	bool			_hasChangedParameters;

public:
	
	SimpleCamera(const VECTOR3& startPosition, const float_t startHeadingRadians);
	virtual ~SimpleCamera();
	
	void forward(const float_t steps);
	void strafe(const float_t steps);
	void up(const float_t steps);
	void changeVelocity(const float_t vel);
	void stopMovement();
	void changeHeading(const float_t degrees);
	void changePitch(const float_t degrees);
	void setPitch(const float_t degrees);
	void moveAxisAligned(const float_t x, const float_t y, const float_t z);
	void reset();
	
	void setFrustum(uint16_t width, uint16_t height, float_t fovInDegree, float_t nearPlane, float_t farPlane);
	void getFrustum(MATRIX &frustum) const;
	void getLookAt(MATRIX &look);
	void getPosition(VECTOR3 &pos) const;
	const VECTOR3* const getPositionPtr() const;
	void getDirection(VECTOR3 &dir) const;
	void getTo(VECTOR3 &to) const;
	void getUp(VECTOR3 &up) const;
	float_t getFieldOfViewInDegree() const;
	float_t getFieldOfViewInRad() const;	
	float_t getZNear() const;
	float_t getZFar() const;
	float_t getAspectRatio() const;
	uint16_t getScreenWidth() const;
	uint16_t getScreenHeight() const;
	
	bool hasChangedParameters();
};


} // namespace MiniGL


#endif // _MINIGL_SIMPLE_CAMERA_H_