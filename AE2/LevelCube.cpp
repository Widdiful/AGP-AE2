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
	m_node->SetCollisionType(SceneNode::Cube);
}
