#include "CoordinateSystem.h"


namespace MiniGL
{
	
	
CoordinateSystem::CoordinateSystem()
: Mesh(6, GL_LINES, false, false)
{
	setVertex(0,  0.0f,  0.0f, -1.0f);
	setVertex(1,  0.0f,  0.0f,  1.0f);
	setVertex(2,  0.0f, -1.0f,  0.0f);
	setVertex(3,  0.0f,  1.0f,  0.0f);
	setVertex(4, -1.0f,  0.0f,  0.0f);
	setVertex(5,  1.0f,  0.0f,  0.0f);
	
	createVertexBufferObjects();
}

	
CoordinateSystem::~CoordinateSystem()
{
}

	
} // namespace MiniGL