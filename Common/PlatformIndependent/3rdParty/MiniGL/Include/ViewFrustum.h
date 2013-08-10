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

#ifndef _MINIGL_VIEW_FRUSTUM_H_
#define _MINIGL_VIEW_FRUSTUM_H_

#include "CrossPlatformHelper.h"
#include "MiniGL.h"
#include "Vector.h"
#include "Matrix.h"
#include "Camera.h"

namespace MiniGL
{

class Camera;

class ViewFrustum
{
	Camera*	const			_camera;
    Vec4					_planes[6];
    Vec3					_frustumPoints[10];
	MATRIX					_projectionMatrix;
	MATRIX					_viewMatrix;
	MATRIX					_viewProjectionMatrix;
	VECTOR3					_cameraPosition;

public:
    
    enum PlaneType
	{
		LEFT_PLANE_ID	=  0,
		RIGHT_PLANE_ID  =  1,
		TOP_PLANE_ID    =  2,
		BOTTOM_PLANE_ID =  3,
		NEAR_PLANE_ID   =  4,
		FAR_PLANE_ID	=  5 
	};
    
    static const int NBL =  0;
    static const int NTL =  1;
    static const int NTR =  2;
    static const int NBR =  3;
    static const int FBL =  4;
    static const int FTL =  5;
    static const int FTR =  6;
    static const int FBR =  7;
    static const int NC  =  8;
    static const int FC  =  9;
	
	enum Relation
	{
		OUT = -1,
		INTERSECT = 0,
		IN = 1
	};
    
	ViewFrustum(MiniGL::Camera* const camera);
	~ViewFrustum();
    
	const Camera* const getCamera() const;
	void updateCameraViewParameterCache();
	const VECTOR3* const getCameraPostionCachePtr() const;
	const MATRIX* const getProjectionMatrixCachePtr() const;
	const MATRIX* const getViewMatrixCachePtr() const;
	const MATRIX* const getViewProjectionMatrixCachePtr() const;
	
	Relation testAgainstPoint(const VECTOR3* const point) const;
	Relation testAgainstSphere(const VECTOR3* const center, const float_t radius) const;
	
	double_t squaredDistanceToCamera(const VECTOR3* const point) const;
	float_t distanceToCamera(const VECTOR3* const point) const;
	float_t screenSpacePixelSize(const VECTOR3* const worldSpacePoint, const float_t innerCircleRadius, float_t* distance) const;
	
	void projectedPointOnScreen(const VECTOR3* const point, int16_t* const x, int16_t* const y) const;
	
	bool isFrontFace(const VECTOR3* const position, const VECTOR3* const normal) const;
};

} // namespace minisg

#endif // _MINISG_VIEW_FRUSTUM_H_