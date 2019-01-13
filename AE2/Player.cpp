#include "Player.h"
#include "SceneNode.h"
#include "Level.h"
#include "MovingPlatform.h"


Player::Player(float useGravity, InputManager* input, SceneNode* camera) : Actor(useGravity)
{
	m_name = "Player";
	m_health = 3;
	
	m_input = input;
	m_camera = camera;

	m_jumpVelocity = 0.05f;
	m_moveSpeed = 0.05f;

	m_coinCount = 0;
	m_redCoinCount = 0;
	m_selectedText = 0;
}


Player::~Player()
{
}

void Player::Start()
{
	Actor::Start();

	m_uiManager = static_cast<UIManager*>(m_node->GetComponent("UI Manager"));
	m_particles = static_cast<Particles*>(m_node->GetComponent("Particles"));
	m_messages.push_back("");
	m_messages.push_back("");

	m_startingParent = m_node->GetParent();
}

void Player::Update()
{
	if (m_rootNode && m_rootNode->GetLevel() && m_rootNode->GetLevel()->IsUsingChestCamera()) {
		return;
	}

	// Move based on keyboard input
	float x = 0, z = 0;
	// Create offset to look at
	if (m_input->IsKeyPressed(DIK_W)) z = 0.5;
	if (m_input->IsKeyPressed(DIK_S)) z = -0.5;
	if (m_input->IsKeyPressed(DIK_A)) x = -0.5;
	if (m_input->IsKeyPressed(DIK_D)) x = 0.5;
	if (x != 0 || z != 0) {
		m_node->LookAt_XZ(m_node->GetXPos() + x, m_node->GetZPos() + z, m_camera->GetYRot());
		m_node->MoveForward(Time::getInstance().deltaTime * m_moveSpeed, m_node->GetRootNode());
		m_moving = true;
	}
	else
		m_moving = false;

	// Manage particles
	if (m_particles) {
		if ((x != 0 || z != 0) && m_grounded) {
			m_particles->UpdatePosition(m_node->GetXPos(), m_node->GetYPos() - m_node->GetModel()->GetBoundingSphereRadius(), m_node->GetZPos());
			m_particles->Play();
		}
		else {
			m_particles->Stop();
		}
	}

	// Jump
	if (m_input->IsKeyBeganPressed(DIK_SPACE) && m_grounded) {
		m_velocityY += m_jumpVelocity;
	}

	// Update UI
	if (m_input->IsKeyBeganPressed(DIK_TAB))
		m_selectedText = (m_selectedText + 1) % m_messages.size();
	m_messages[0] = "H" + std::to_string(m_health) + " C" + std::to_string(m_coinCount) + "O" + std::to_string(m_level->GetCoinCount()) + " R" + std::to_string(m_redCoinCount) + "O" + std::to_string(m_level->GetRedCoinCount());
	m_messages[1] = "X" + to_string((int)(floorf(m_node->GetXPos() * 100)) / 100) + " Y" + to_string((int)(floorf(m_node->GetYPos() * 100)) / 100) + " Z" + to_string((int)(floorf(m_node->GetZPos() * 100)) / 100) + " F" + to_string((int)(floorf(Time::getInstance().fps * 100)) / 100);
	replace(m_messages[1].begin(), m_messages[1].end(), '-', 'M');
	if (m_uiManager) m_uiManager->ChangeText(m_messages[m_selectedText]);

	if (m_node->GetParent() != m_startingParent) {
		//m_node->SetParent(m_startingParent);
	}


	Actor::Update();
}

void Player::OnCollision(SceneNode * other)
{
	Actor::OnCollision(other);
	// Collision code based on node name
	if (other->GetName() == "Enemy") {
		if (m_node->GetYPos() > other->GetYPos() && m_velocityY < 0) {
			if (static_cast<Actor*>(other->GetComponent("Enemy"))) {
				static_cast<Actor*>(other->GetComponent("Enemy"))->TakeDamage();
				m_coinCount++;
			}
		}
		else if (m_iframes <= 0) {
			m_iframes = 1;
			TakeDamage();
		}
	}
	else if (other->GetName() == "Coin") {
		m_coinCount++;
		other->SetEnabled(false);
	}
	else if (other->GetName() == "RedCoin") {
		m_redCoinCount++;
		other->SetEnabled(false);
		if (m_redCoinCount == m_level->GetRedCoinCount()) {
			m_level->SpawnChest();
		}
	}
	else if (other->GetName() == "Chest") {
		m_level->CompleteLevel();
	}
}

void Player::TakeDamage()
{
	m_health--;
	if (m_health <= 0) {
		m_uiManager->ChangeText("Press r to retry");
		m_node->SetEnabled(false);
	}
}
