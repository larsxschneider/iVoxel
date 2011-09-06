#include "SceneNode.h"


namespace MiniGL
{
	
	
SceneNode::SceneNode()
{	
	_id = 0;
	_isActive = true;
}

	
SceneNode::~SceneNode()
{
}

	
void SceneNode::setId(int id)
{
	_id = id;
}

	
int SceneNode::getId()
{
	return _id;
}

	
bool SceneNode::isActive()
{
	return _isActive;
}
	

void SceneNode::setActive(bool active) 
{
	_isActive = active;
}
	
	
} // namespace MiniGL
