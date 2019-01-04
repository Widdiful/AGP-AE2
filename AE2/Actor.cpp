#include "Actor.h"
#include "scene_node.h"



Actor::Actor()
{
	m_velocityX = 0;
	m_velocityY = 0;
	m_velocityZ = 0;

	m_gravity = -0.0001f;
	m_gravityMax = -0.01f;
	m_useGravity = true;
}

Actor::~Actor()
{
}

void Actor::Start()
{
	Component::Start();
}

void Actor::Update()
{
	Component::Update();

	if (m_useGravity) {
		if (m_velocityY > m_gravityMax) m_velocityY += m_gravity;
	}

	if (m_node) {
		m_node->AddXPos(m_velocityX, m_rootNode);
		m_node->AddYPos(m_velocityY, m_rootNode);
		m_node->AddZPos(m_velocityZ, m_rootNode);
	}
}
