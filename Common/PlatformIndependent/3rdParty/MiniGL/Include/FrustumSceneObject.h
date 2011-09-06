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