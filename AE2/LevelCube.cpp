#include "LevelCube.h"
#include "SceneNode.h"



LevelCube::LevelCube()
{
}


LevelCube::~LevelCube()
{
}

void LevelCube::Start()
{
	// Set collider to cube
	m_node->SetCollisionType(SceneNode::Cube);
}
