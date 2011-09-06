#ifndef _MINIGL_PLANE_H_
#define _MINIGL_PLANE_H_


#include "Mesh.h"


namespace MiniGL
{
	
	
class Plane : public Mesh
{
	
public:
	
	Plane(GLuint textureHandle, bool centered = true);
	virtual ~Plane();
};
	
	
} // namespace MiniGL

#endif // _MINIGL_PLANE_H_