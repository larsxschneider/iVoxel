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
    
    typedef enum PlaneType
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
	
	typedef enum Relation
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