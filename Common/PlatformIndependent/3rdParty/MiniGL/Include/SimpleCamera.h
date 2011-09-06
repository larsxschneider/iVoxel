/*
 *  SimpleCamera.h
 *  MacApp
 *
 *  Created by Lars Schneider on 30.10.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
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