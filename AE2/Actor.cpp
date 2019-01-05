#include "Actor.h"
#include "scene_node.h"



Actor::Actor(float useGravity)
{
	m_velocityX = 0;
	m_velocityY = 0;
	m_velocityZ = 0;

	m_gravity = -0.00002f;
	m_gravityMax = -0.1f;
	m_useGravity = useGravity;
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

	if (m_node) {
		m_node->AddXPos(m_velocityX, m_rootNode);
		if (m_node->AddYPos(m_velocityY, m_rootNode)) {
			m_velocityY = 0;
		}
		else if (m_useGravity) {
			if (m_velocityY > m_gravityMax) m_velocityY += m_gravity;
		}
		m_node->AddZPos(m_velocityZ, m_rootNode);

		if (m_node->GetYPos() < 0) {
			m_node->SetYPos(0);
			if (m_velocityY < 0) {
				m_velocityY = 0;
			}
		}
	}
}
