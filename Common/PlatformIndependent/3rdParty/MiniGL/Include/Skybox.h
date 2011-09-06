#ifndef _MINIGL_SKYBOX_H_
#define _MINIGL_SKYBOX_H_


#include "Mesh.h"


namespace MiniGL
{
	
	
class Skybox : public Mesh
{
	int	_planes;
	
public:
	
	Skybox(float scale, bool adjustUV, int textureSize, GLuint* textureHandle, bool withGroundPlane);
	virtual ~Skybox();
	
	void render();
};
	
	
} // namespace MiniGL

#endif // _MINIGL_SKYBOX_H_