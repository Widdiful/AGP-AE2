#include "Actor.h"
#include "SceneNode.h"
#include "Level.h"
#include "MovingPlatform.h"

const float platformYoffset = 1.01f;
const float platformYoffsetMoving = 0.19f;

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
	if (m_rootNode && m_rootNode->GetLevel() && m_rootNode->GetLevel()->IsUsingChestCamera()) {
		return;
	}
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

void Actor::OnCollision(SceneNode * other)
{
	if (other->GetName() == "MovingPlatform") {
		Vector3 movement = static_cast<MovingPlatform*>(other->GetComponent("Moving Platform"))->GetMovementInfo();
		m_node->AddXPos(movement.x * 2 * Time::getInstance().deltaTime);
		m_node->AddYPos(movement.y * 2 * Time::getInstance().deltaTime);
		m_node->AddZPos(movement.z * 2 * Time::getInstance().deltaTime);
		if (movement.y > 0) {
			m_node->SetYPos(other->GetYPos() + other->GetModel()->GetCubeBounds().y + m_node->GetModel()->GetBoundingSphereRadius() + platformYoffset);
			if (m_moving) m_node->AddYPos(platformYoffsetMoving);
		}
	}
}
