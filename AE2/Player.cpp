#include "Player.h"
#include "SceneNode.h"
#include "Level.h"


Player::Player(float useGravity, InputManager* input, SceneNode* camera) : Actor(useGravity)
{
	m_name = "Player";
	
	m_input = input;
	m_camera = camera;

	m_jumpVelocity = 0.05f;
	m_moveSpeed = 0.05f;

	m_coinCount = 0;
	m_redCoinCount = 0;
}


Player::~Player()
{
}

void Player::Start()
{
	Actor::Start();

	m_uiManager = static_cast<UIManager*>(m_node->GetComponent("UI Manager"));
}

void Player::Update()
{
	float x = 0, z = 0;
	if (m_input->IsKeyPressed(DIK_W)) z = 0.5;
	if (m_input->IsKeyPressed(DIK_S)) z = -0.5;
	if (m_input->IsKeyPressed(DIK_A)) x = -0.5;
	if (m_input->IsKeyPressed(DIK_D)) x = 0.5;
	if (x != 0 || z != 0) {
		m_node->LookAt_XZ(m_node->GetXPos() + x, m_node->GetZPos() + z, m_camera->GetYRot());
		m_node->MoveForward(m_level->GetDeltaTime() * m_moveSpeed, m_node->GetRootNode());
	}

	if (m_input->IsKeyBeganPressed(DIK_SPACE)) {
		m_velocityY += m_jumpVelocity;
	}

	m_uiManager->ChangeText("Coins: " + std::to_string(m_coinCount) + " Red coins: " + std::to_string(m_redCoinCount));

	Actor::Update();
}

void Player::OnCollision(SceneNode * other)
{
	if (other->GetName() == "Enemy") {
		// dies
	}
	else if (other->GetName() == "Coin") {
		m_coinCount++;
		other->SetEnabled(false);
	}
	else if (other->GetName() == "Red Coin") {
		m_redCoinCount++;
		other->SetEnabled(false);
	}
}
