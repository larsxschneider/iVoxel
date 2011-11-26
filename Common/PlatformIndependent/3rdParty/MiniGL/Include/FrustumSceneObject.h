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

#ifndef _MINIGL_FRUSTUM_SCENE_OBJECT_H_
#define _MINIGL_FRUSTUM_SCENE_OBJECT_H_


#include "Mesh.h"
#include "Vector.h"
#include "CrossPlatformHelper.h"

namespace MiniGL
{

class Camera;
	
class FrustumSceneObject : public Mesh
{
    Vec3 _frustumPoints[10];
			
	static const bool BEHIND    = false;
    static const bool INFRONT   = true;
    
    static const int LEFT       =  0;
    static const int RIGHT      =  1;
    static const int TOP        =  2;
    static const int BOTTOM     =  3;
    static const int NEAR       =  4;
    static const int FAR        =  5;
    
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
    
    static const int INSIDE     =  1;
    static const int INTERSECT  =  0;
    static const int OUTSIDE    = -1;
	
public:
	
	FrustumSceneObject(Camera *camera);
	virtual ~FrustumSceneObject();
};
	
	
} // namespace MiniGL

#endif // _MINIGL_FRUSTUM_SCENE_OBJECT_H_