#ifndef _MINIGL_SCENENODE_H_
#define _MINIGL_SCENENODE_H_

#include "PlatformOpenGL.h"
#include "Constants.h"


namespace MiniGL
{
	
	
class SceneNode
{
	int					_id;

protected:
	
	bool				_isActive;
    GLuint				*_dataVBO,
						*_indexVBO,
						*_textureID;
//	GLsizei				_numberOfVertices;
//    GLenum				_drawMode;
	
public:
	
	SceneNode();
	virtual ~SceneNode();
	
	void setId(int id);
	int getId();
	
    bool isActive();
	void setActive(bool active);

};
	
	
} // namespace MiniGL


#endif // _MINIGL_SCENENODE_H_
