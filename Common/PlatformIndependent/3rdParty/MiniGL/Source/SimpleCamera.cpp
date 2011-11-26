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

#include "SimpleCamera.h"


namespace MiniGL
{
		
		
SimpleCamera::SimpleCamera(const VECTOR3& startPosition, const float_t startHeadingRadians)
  : Camera(),
	_startHeadingRadians(startHeadingRadians),
	_maxPitchRate(DEGREES_TO_RADIANS(2.0f)),
	_maxHeadingRate(DEGREES_TO_RADIANS(16.0f)),
	_maxForwardVelocity(100.0f)
{
	_hasChangedParameters	= true;

	_up = Vec3(0.0f, 1.0f, 0.0f);
	_startPosition.x = startPosition.x;
	_startPosition.y = startPosition.y;
	_startPosition.z = startPosition.z;
		
	reset();
}


SimpleCamera::~SimpleCamera()
{
}


void SimpleCamera::reset()
{
	_position			= _startPosition;
	_headingRadians		= _startHeadingRadians;
	_pitchRadians		= 0.0f;
	_forwardVelocity	= 0.0f;
	_hasChangedParameters = true;
}


void SimpleCamera::getPosition(VECTOR3 &pos) const
{
	pos = _position;
}


const VECTOR3* const SimpleCamera::getPositionPtr() const
{
	return &_position;
}


void SimpleCamera::getDirection(VECTOR3 &dir) const
{
	dir = _direction;
}


void SimpleCamera::getTo(VECTOR3 &to) const
{
	// Direction is magnified to avoid rounding errors with WPVS
	to = _position + _direction * 100000.0f;
}


void SimpleCamera::getUp(VECTOR3 &up) const
{
	up = _up;
}


void SimpleCamera::setFrustum(uint16_t width, uint16_t height, float_t fovInDegree, float_t nearPlane, float_t farPlane)
{
	_fovInRad = DEGREES_TO_RADIANS(fovInDegree);
	_zNear = nearPlane;
	_zFar = farPlane;
	_screenWidth = width;
	_screenHeight = height;
	_aspectRatio = float_t(_screenWidth) / float_t(_screenHeight);
	
	MatrixPerspectiveFovRH(
		_projectionMatrix, 
		_fovInRad, 
		_aspectRatio,
		_zNear,
		_zFar,
		0);
		
	_hasChangedParameters = true;
}


float_t SimpleCamera::getFieldOfViewInDegree() const
{
	return RADIANS_TO_DEGREES(_fovInRad);
}


uint16_t SimpleCamera::getScreenWidth() const
{
	return _screenWidth;
}


uint16_t SimpleCamera::getScreenHeight() const
{
	return _screenHeight;
}


float_t SimpleCamera::getAspectRatio() const
{
	return _aspectRatio;
}


float_t SimpleCamera::getZNear() const
{
	return _zNear;
}


float_t SimpleCamera::getZFar() const
{
	return _zFar;
}


float_t SimpleCamera::getFieldOfViewInRad() const
{
	return _fovInRad;
}


void SimpleCamera::getFrustum(MATRIX &frustum) const
{
	frustum = _projectionMatrix;
}


//see http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=Quaternion_Camera_Class
void SimpleCamera::getLookAt(MATRIX &look)
{
	MATRIX m1, m2, m3, translation;
	QUATERNION q;

	// Make the Quaternions that will represent our rotations
	MatrixQuaternionRotationAxis(_qPitch, Vec3(1.0f, 0.0f, 0.0f), _pitchRadians);
	MatrixQuaternionRotationAxis(_qHeading, Vec3(0.0f, 1.0f, 0.0f), _headingRadians);

	// Combine the pitch and heading rotations and store the results in q
	MatrixQuaternionMultiply(q, _qHeading, _qPitch);
	MatrixRotationQuaternion(m1, q);
	
	// Create a matrix from the pitch Quaternion and get the j vector 
	// for our direction.
	MatrixRotationQuaternion(m3, _qPitch);
	_direction.y = m3.f[9];

	// Combine the heading and pitch rotations and make a matrix to get
	// the i and j vectors for our direction.
	MatrixQuaternionMultiply(q, _qPitch, _qHeading);
	MatrixRotationQuaternion(m2, q);
	_direction.x = m2.f[8];
	_direction.z = -m2.f[10];

	// Increment our position by the vector
	_position += (_direction * _forwardVelocity);

	// Translate to our new position.
	MatrixTranslation(translation, -_position.x, -_position.y, -_position.z);
	MatrixMultiply(look, translation, m1);
}


void SimpleCamera::forward(const float_t steps)
{
	_position += (_direction * _maxForwardVelocity * steps);
	_hasChangedParameters = true;
}


void SimpleCamera::strafe(const float_t steps)
{
	Vec3 sideDir;
	MatrixVec3CrossProduct(sideDir, _direction, _up);
	_position += (sideDir * _maxForwardVelocity * steps);
	_hasChangedParameters = true;
}


void SimpleCamera::up(const float_t steps)
{
	_position += (_up * _maxForwardVelocity * steps);
	_hasChangedParameters = true;
}


void SimpleCamera::moveAxisAligned(const float_t x, const float_t y, const float_t z)
{
	_position.x += (x * _maxForwardVelocity);
	_position.y += (y * _maxForwardVelocity);
	_position.z += (z * _maxForwardVelocity);
	_hasChangedParameters = true;
}
	
	
void SimpleCamera::setPitch(const float_t degrees)
{
	_pitchRadians = degrees;
	_hasChangedParameters = true;
}


void SimpleCamera::changePitch(const float_t degrees)
{
	float_t changeValue = DEGREES_TO_RADIANS(degrees);
	if(fabs(changeValue) < fabs(_maxPitchRate))
	{
		// Our pitch is less than the max pitch rate that we 
		// defined so lets increment it.
		_pitchRadians += changeValue;
	}
	else
	{
		// Our pitch is greater than the max pitch rate that
		// we defined so we can only increment our pitch by the 
		// maximum allowed value.
		if(changeValue < 0)
		{
			// We are pitching down so decrement
			_pitchRadians -= _maxPitchRate;
		}
		else
		{
			// We are pitching up so increment
			_pitchRadians += _maxPitchRate;
		}
	}

	// We don't want our pitch to run away from us. Although it
	// really doesn't matter I prefer to have my pitch degrees
	// within the range of -360.0f to 360.0f
	if(_pitchRadians > DEGREES_TO_RADIANS(360.0f))
	{
		_pitchRadians -= DEGREES_TO_RADIANS(360.0f);
	}
	else if(_pitchRadians < DEGREES_TO_RADIANS(-360.0f))
	{
		_pitchRadians += DEGREES_TO_RADIANS(360.0f);
	}

	_hasChangedParameters = true;
}


void SimpleCamera::changeHeading(const float_t degrees)
{
	float_t changeValue = DEGREES_TO_RADIANS(degrees);
	if(fabs(changeValue) < fabs(_maxHeadingRate))
	{
		// Our Heading is less than the max heading rate that we 
		// defined so lets increment it but first we must check
		// to see if we are inverted so that our heading will not
		// become inverted.
		if(	(_pitchRadians > DEGREES_TO_RADIANS(90) && 
			 _pitchRadians < DEGREES_TO_RADIANS(270)) || 
			(_pitchRadians < DEGREES_TO_RADIANS(-90) && 
			 _pitchRadians > DEGREES_TO_RADIANS(-270)))
		{
			_headingRadians -= changeValue;
		}
		else
		{
			_headingRadians += changeValue;
		}
	}
	else
	{
		// Our heading is greater than the max heading rate that
		// we defined so we can only increment our heading by the 
		// maximum allowed value.
		if(changeValue < 0)
		{
			// Check to see if we are upside down.
			if((_pitchRadians > DEGREES_TO_RADIANS(90) && 
				_pitchRadians < DEGREES_TO_RADIANS(270)) || 
			   (_pitchRadians < DEGREES_TO_RADIANS(-90) && 
				_pitchRadians > DEGREES_TO_RADIANS(-270)))
			{
				// Ok we would normally decrement here but since we are upside
				// down then we need to increment our heading
				_headingRadians += _maxHeadingRate;
			}
			else
			{
				// We are not upside down so decrement as usual
				_headingRadians -= _maxHeadingRate;
			}
		}
		else
		{
			// Check to see if we are upside down.
			if(	(_pitchRadians > DEGREES_TO_RADIANS(90) && 
				 _pitchRadians < DEGREES_TO_RADIANS(270)) || 
				(_pitchRadians < DEGREES_TO_RADIANS(-90) && 
				 _pitchRadians > DEGREES_TO_RADIANS(-270)))
			{
				// Ok we would normally increment here but since we are upside
				// down then we need to decrement our heading.
				_headingRadians -= _maxHeadingRate;
			}
			else
			{
				// We are not upside down so increment as usual.
				_headingRadians += _maxHeadingRate;
			}
		}
	}
	
	// We don't want our heading to run away from us either. Although it
	// really doesn't matter I prefer to have my heading degrees
	// within the range of -360.0f to 360.0f
	if(_headingRadians > DEGREES_TO_RADIANS(360.0f))
	{
		_headingRadians -= DEGREES_TO_RADIANS(360.0f);
	}
	else if(_headingRadians < DEGREES_TO_RADIANS(-360.0f))
	{
		_headingRadians += DEGREES_TO_RADIANS(360.0f);
	}
	
	_hasChangedParameters = true;
}


void SimpleCamera::changeVelocity(const float_t vel)
{
	if(fabs(vel) < fabs(_maxForwardVelocity))
	{
		// Our velocity is less than the max velocity increment that we 
		// defined so lets increment it.
		_maxForwardVelocity += vel;
	}
	else
	{
		// Our velocity is greater than the max velocity increment that
		// we defined so we can only increment our velocity by the 
		// maximum allowed value.
		if(vel < 0)
		{
			// We are slowing down so decrement
			_forwardVelocity -= _maxForwardVelocity;
		}
		else
		{
			// We are speeding up so increment
			_forwardVelocity += _maxForwardVelocity;
		}
	}
	
	_hasChangedParameters = true;
}


void SimpleCamera::stopMovement()
{
	_forwardVelocity = 0.0f;
}
	

bool SimpleCamera::hasChangedParameters()
{
	// We report changes only once
	if (_hasChangedParameters)
	{
		_hasChangedParameters = false;
		return true;
	}
	return false;
}


} // namespace MiniGL