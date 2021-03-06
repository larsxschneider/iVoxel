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

#include "FrustumSceneObject.h"
#include "Camera.h"


namespace MiniGL
{
	
	
FrustumSceneObject::FrustumSceneObject(Camera *camera)
: Mesh(8, GL_LINES, false, false)
{
	MATRIX lookAt;
	Vec3 x, y, z, lookFrom;
    float nw, nh, tang, fh, fw;
	
	tang = (float)tan(camera->getFieldOfViewInRad() * 0.5);

	camera->getPosition(lookFrom);
	camera->getLookAt(lookAt);
	
	x.x = lookAt.f[ 0];
	x.y = lookAt.f[ 4];
	x.z = lookAt.f[ 8];
	
	y.x = lookAt.f[ 1];
	y.y = lookAt.f[ 5];
	y.z = lookAt.f[ 9];
	
	z.x = lookAt.f[ 2];
	z.y = lookAt.f[ 6];
	z.z = lookAt.f[10];

    // compute near and far plane width and hight
    nh = camera->getZNear() * tang;
    nw = nh * camera->getAspectRatio(); 
    fh = camera->getZFar() * tang;
    fw = fh * camera->getAspectRatio();

    // compute the centers of the near and far planes
    _frustumPoints[NC] = lookFrom - z * camera->getZNear();
    _frustumPoints[FC] = lookFrom - z * camera->getZFar();

    // compute the 4 corners of the frustum on the near plane
    _frustumPoints[NBL] = _frustumPoints[NC] + y * nh - x * nw;
    _frustumPoints[NBR] = _frustumPoints[NC] + y * nh + x * nw;
    _frustumPoints[NTL] = _frustumPoints[NC] - y * nh - x * nw;
    _frustumPoints[NTR] = _frustumPoints[NC] - y * nh + x * nw;
    
    // compute the 4 corners of the frustum on the far plane
    _frustumPoints[FBL] = _frustumPoints[FC] + y * fh - x * fw;
    _frustumPoints[FBR] = _frustumPoints[FC] + y * fh + x * fw;
    _frustumPoints[FTL] = _frustumPoints[FC] - y * fh - x * fw;
    _frustumPoints[FTR] = _frustumPoints[FC] - y * fh + x * fw;

	setVertex(0,  _frustumPoints[NBL].x, _frustumPoints[NBL].y, _frustumPoints[NBL].z);
	setVertex(1,  _frustumPoints[FBL].x, _frustumPoints[FBL].y, _frustumPoints[FBL].z);
	setVertex(2,  _frustumPoints[NBR].x, _frustumPoints[NBR].y, _frustumPoints[NBR].z);
	setVertex(3,  _frustumPoints[FBR].x, _frustumPoints[FBR].y, _frustumPoints[FBR].z);
	
	setVertex(4,  _frustumPoints[NTL].x, _frustumPoints[NTL].y, _frustumPoints[NTL].z);
	setVertex(5,  _frustumPoints[FTL].x, _frustumPoints[FTL].y, _frustumPoints[FTL].z);
	setVertex(6,  _frustumPoints[NTR].x, _frustumPoints[NTR].y, _frustumPoints[NTR].z);
	setVertex(7,  _frustumPoints[FTR].x, _frustumPoints[FTR].y, _frustumPoints[FTR].z);

	createVertexBufferObjects();
}

	
FrustumSceneObject::~FrustumSceneObject()
{
}

	
} // namespace MiniGL
