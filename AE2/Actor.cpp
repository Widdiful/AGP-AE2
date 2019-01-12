#include "Actor.h"
#include "SceneNode.h"
#include "Level.h"



Actor::Actor(float useGravity)
{
	m_name = "Actor";
	m_health = 1;
	m_visible = true;

	m_velocityX = 0;
	m_velocityY = 0;
	m_velocityZ = 0;

	m_gravity = -0.0001f;
	m_gravityMax = -0.5f;
	m_useGravity = useGravity;
	m_deathY = -50;
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

	// Apply velocity / gravity
	if (m_node) {

		m_node->AddXPos(m_velocityX * Time::getInstance().deltaTime, m_rootNode);
		m_node->AddZPos(m_velocityZ * Time::getInstance().deltaTime, m_rootNode);

		// Gravity
		if (m_node->AddYPos(m_velocityY * Time::getInstance().deltaTime, m_rootNode)) {
			m_velocityY = 0;
			m_grounded = true;
		}
		else if (m_useGravity) {
			if (m_velocityY > m_gravityMax) m_velocityY += m_gravity * Time::getInstance().deltaTime;
		}

		if (m_velocityY > 0.01f || m_velocityY < -0.01f) {
			m_grounded = false;
		}

		/*// Hard coded ground level
		if (m_node->GetYPos() < 0) {
			m_node->SetYPos(0);
			if (m_velocityY < 0) {
				m_velocityY = 0;
				m_grounded = true;
			}
		}*/

		bool blinking = false;
		if (m_iframes > 0) {
			m_node->SetVisible(!m_node->GetVisible());
			m_iframes -= Time::getInstance().deltaTime / 1000.0f;
			blinking = true;
		}
		if (m_iframes <= 0 && blinking) {
			m_node->SetVisible(m_visible);
		}

		if (m_node->GetYPos() <= m_deathY) {
			TakeDamage();
		}
	}
}

void Actor::TakeDamage()
{
	m_health--;
	if (m_health <= 0) {
		m_node->SetEnabled(false);
	}
}
