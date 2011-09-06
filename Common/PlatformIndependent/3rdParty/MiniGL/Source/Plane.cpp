#include "Plane.h"


namespace MiniGL
{
	
	
Plane::Plane(GLuint textureHandle, bool centered)
: Mesh(4, GL_TRIANGLE_STRIP, true, true)
{
	if (centered)
	{
		setVertex(0, +0.5f, -0.5f, 0.0f);
		setVertex(1, +0.5f, +0.5f, 0.0f);
		setVertex(2, -0.5f, -0.5f, 0.0f);
		setVertex(3, -0.5f, +0.5f, 0.0f);
	}
	else
	{
		setVertex(0, 1.0f, 0.0f, 0.0f);
		setVertex(1, 1.0f, 1.0f, 0.0f);
		setVertex(2, 0.0f, 0.0f, 0.0f);
		setVertex(3, 0.0f, 1.0f, 0.0f);
	}
	
	for (int i=0; i<3; i++)
	{
		setNormal(i, 0.0f, 0.0f, 1.0f);
	}
	
	setUV(0, 1.0f, 0.0f);
	setUV(1, 1.0f, 1.0f);
	setUV(2, 0.0f, 0.0f);
	setUV(3, 0.0f, 1.0f);
	
	createVertexBufferObjects();
	
	createTextureHandleArray(1);
	setTextureHandle(0, textureHandle);
}

	
Plane::~Plane()
{
}

	
} // namespace MiniGL
