#pragma once
#include "Actor.h"
#include "InputManager.h"
#include "Camera.h"
#include "UIManager.h"
#include "Particles.h"
class Player :
	public Actor
{
private:
	InputManager* m_input;
	SceneNode* m_camera;
	SceneNode* m_startingParent;

	UIManager* m_uiManager;
	std::string m_uiMessage;
	Particles* m_particles;
	int m_coinCount, m_redCoinCount;
	int m_selectedText;
	vector<string> m_messages;
	float m_platformY = 0;
	bool m_onPlatform = false;
public:
	Player(float useGravity, InputManager* input, SceneNode* camera);
	~Player();

	void Start();
	void Update();
	void OnCollision(SceneNode* other);
	void TakeDamage();
};

