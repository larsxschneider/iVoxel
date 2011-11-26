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

#include "ViewFrustum.h"
#include "Camera.h"
#include <stdio.h>


namespace MiniGL
{


ViewFrustum::ViewFrustum(Camera* const camera) :
	_camera(camera)
{
}


ViewFrustum::~ViewFrustum()
{
}


const Camera* const ViewFrustum::getCamera() const
{
	return _camera;
}


const VECTOR3* const ViewFrustum::getCameraPostionCachePtr() const
{
	return &_cameraPosition;
}


const MATRIX* const ViewFrustum::getViewProjectionMatrixCachePtr() const
{
	return &_viewProjectionMatrix;
}


const MATRIX* const ViewFrustum::getProjectionMatrixCachePtr() const
{
	return &_projectionMatrix;
}


const MATRIX* const ViewFrustum::getViewMatrixCachePtr() const
{
	return &_viewMatrix;
}


void ViewFrustum::updateCameraViewParameterCache()
{
	// Retrieve latest camera parameter
	_camera->getPosition(_cameraPosition);
	_camera->getLookAt(_viewMatrix);	
	_camera->getFrustum(_projectionMatrix);

	// Calc temporary values
	MatrixMultiply(_viewProjectionMatrix, _viewMatrix, _projectionMatrix);

	// Left clipping plane
	_planes[LEFT_PLANE_ID].x = _viewProjectionMatrix.f[3] + _viewProjectionMatrix.f[0]; 
	_planes[LEFT_PLANE_ID].y = _viewProjectionMatrix.f[7] + _viewProjectionMatrix.f[4]; 
	_planes[LEFT_PLANE_ID].z = _viewProjectionMatrix.f[11] + _viewProjectionMatrix.f[8]; 
	_planes[LEFT_PLANE_ID].w = _viewProjectionMatrix.f[15] + _viewProjectionMatrix.f[12]; 

	// Right clipping plane 
	_planes[RIGHT_PLANE_ID].x = _viewProjectionMatrix.f[3] - _viewProjectionMatrix.f[0]; 
	_planes[RIGHT_PLANE_ID].y = _viewProjectionMatrix.f[7] - _viewProjectionMatrix.f[4]; 
	_planes[RIGHT_PLANE_ID].z = _viewProjectionMatrix.f[11] - _viewProjectionMatrix.f[8]; 
	_planes[RIGHT_PLANE_ID].w = _viewProjectionMatrix.f[15] - _viewProjectionMatrix.f[12]; 

	// Top clipping plane 
	_planes[TOP_PLANE_ID].x = _viewProjectionMatrix.f[3] - _viewProjectionMatrix.f[1]; 
	_planes[TOP_PLANE_ID].y = _viewProjectionMatrix.f[7] - _viewProjectionMatrix.f[5]; 
	_planes[TOP_PLANE_ID].z = _viewProjectionMatrix.f[11] - _viewProjectionMatrix.f[9]; 
	_planes[TOP_PLANE_ID].w = _viewProjectionMatrix.f[15] - _viewProjectionMatrix.f[13]; 

	// Bottom clipping plane 
	_planes[BOTTOM_PLANE_ID].x = _viewProjectionMatrix.f[3] + _viewProjectionMatrix.f[1]; 
	_planes[BOTTOM_PLANE_ID].y = _viewProjectionMatrix.f[7] + _viewProjectionMatrix.f[5]; 
	_planes[BOTTOM_PLANE_ID].z = _viewProjectionMatrix.f[11] + _viewProjectionMatrix.f[9]; 
	_planes[BOTTOM_PLANE_ID].w = _viewProjectionMatrix.f[15] + _viewProjectionMatrix.f[13]; 

	// Near clipping plane 
	_planes[NEAR_PLANE_ID].x = _viewProjectionMatrix.f[3] + _viewProjectionMatrix.f[2]; 
	_planes[NEAR_PLANE_ID].y = _viewProjectionMatrix.f[7] + _viewProjectionMatrix.f[6]; 
	_planes[NEAR_PLANE_ID].z = _viewProjectionMatrix.f[11] + _viewProjectionMatrix.f[10]; 
	_planes[NEAR_PLANE_ID].w = _viewProjectionMatrix.f[15] + _viewProjectionMatrix.f[14]; 

	// Far clipping plane 
	_planes[FAR_PLANE_ID].x = _viewProjectionMatrix.f[3] - _viewProjectionMatrix.f[2]; 
	_planes[FAR_PLANE_ID].y = _viewProjectionMatrix.f[7] - _viewProjectionMatrix.f[6]; 
	_planes[FAR_PLANE_ID].z = _viewProjectionMatrix.f[11] - _viewProjectionMatrix.f[10]; 
	_planes[FAR_PLANE_ID].w = _viewProjectionMatrix.f[15] - _viewProjectionMatrix.f[14]; 

	// Normalize the plane equations 
	for (int p = 0; p < 6; p++) 
	{
		MatrixVec4Normalize(_planes[p], _planes[p]);
	}
}


ViewFrustum::Relation ViewFrustum::testAgainstPoint(const VECTOR3* const point) const
{
    for (int i=0; i < 6; i++) 
    {
        if (_planes[i].x * point->x + 
			_planes[i].y * point->y + 
			_planes[i].z * point->z +
			_planes[i].w <= 0.0f)
		{
			return OUT;
		}
    }
    
    return IN;
}


ViewFrustum::Relation ViewFrustum::testAgainstSphere(
	const VECTOR3* const center,
	const float_t radius) const
{
	float_t distance;
	Relation result = IN;
	
	// calculate our distances to each of the planes
	for(int i = 0; i < 6; ++i)
	{
		// find the distance to this plane (via dot product)
		distance =	_planes[i].x * center->x + 
					_planes[i].y * center->y + 
					_planes[i].z * center->z +
					_planes[i].w;

		// if this distance is < -sphere radius, we are outside
		if (distance < -radius)	return OUT;

		// else if the distance is between +- radius, then we intersect
		if(distance < radius) result = INTERSECT;
	}

	return result;
}


// see Efficient Implementation of Real-Time View-Dependent Multiresolution Meshing
// Pajarola, DeCoro 2004
float_t ViewFrustum::screenSpacePixelSize(const VECTOR3* const point, const float_t innerCircleRadius, float_t* distance) const
{
	VECTOR3 a;
	a.x = point->x - _cameraPosition.x;
	a.y = point->y - _cameraPosition.y;
	a.z = point->z - _cameraPosition.z;
	
	return innerCircleRadius * innerCircleRadius / (a.x * a.x + a.y * a.y + a.z * a.z);
}


double_t ViewFrustum::squaredDistanceToCamera(const VECTOR3* const point) const
{
	const double_t x = point->x - _cameraPosition.x;
	const double_t y = point->y - _cameraPosition.y;
	const double_t z = point->z - _cameraPosition.z;
	
	return (x * x + y * y + z * z);
}


void ViewFrustum::projectedPointOnScreen(const VECTOR3* const point, int16_t* const x, int16_t* const y) const
{
	// Precalc invert of w
	float_t wInv =	1.0f / (_viewProjectionMatrix.f[_14] * point->x + 
							_viewProjectionMatrix.f[_24] * point->y + 
							_viewProjectionMatrix.f[_34] * point->z + 
							_viewProjectionMatrix.f[_44]);
				
	*x =	float_t(_camera->getScreenWidth())/2.0f *
			((_viewProjectionMatrix.f[_11] * point->x + 
			_viewProjectionMatrix.f[_21] * point->y + 
			_viewProjectionMatrix.f[_31] * point->z + 
			_viewProjectionMatrix.f[_41]) * wInv + 1.0f);
		

	*y =	float_t(_camera->getScreenHeight())/2.0f *
			((_viewProjectionMatrix.f[_12] * point->x + 
			_viewProjectionMatrix.f[_22] * point->y + 
			_viewProjectionMatrix.f[_32] * point->z + 
			_viewProjectionMatrix.f[_42]) * wInv + 1.0f);
}
 

float_t ViewFrustum::distanceToCamera(const VECTOR3* const point) const
{
	return -(	_viewMatrix.f[_13] * point->x + 
				_viewMatrix.f[_23] * point->y + 
				_viewMatrix.f[_33] * point->z + 
				_viewMatrix.f[_43]);
}


bool ViewFrustum::isFrontFace(const VECTOR3* const position, const VECTOR3* const normal) const
{
	// See Koppitz Diplomarbeit, Seite 34/35
	VECTOR3 pv;
	pv.x = position->x - _cameraPosition.x;
	pv.y = position->y - _cameraPosition.y;
	pv.z = position->z - _cameraPosition.z;
	
	return (MatrixVec3DotProduct(*normal, pv) < 0.0f);
}


}