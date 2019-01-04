#include "Player.h"
#include "scene_node.h"


Player::Player(float useGravity, InputManager* input) : Actor(useGravity)
{
	m_input = input;

	m_jumpVelocity = 0.05f;
}


Player::~Player()
{
}

void Player::Start()
{
	Actor::Start();
}

void Player::Update()
{
	float x = 0, z = 0;
	if (m_input->IsKeyPressed(DIK_W)) z = 0.5;
	if (m_input->IsKeyPressed(DIK_S)) z = -0.5;
	if (m_input->IsKeyPressed(DIK_A)) x = -0.5;
	if (m_input->IsKeyPressed(DIK_D)) x = 0.5;
	if (x != 0 || z != 0) {
		m_node->LookAt_XZ(m_node->GetXPos() + x, m_node->GetZPos() + z);
		m_node->MoveForward(0.005, m_node->GetRootNode());
	}

	if (m_input->IsKeyBeganPressed(DIK_SPACE)) {
		m_velocityY += m_jumpVelocity;
	}

	Actor::Update();
}
