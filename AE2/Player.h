#pragma once
#include "Actor.h"
#include "InputManager.h"
#include "Camera.h"
#include "UIManager.h"
class Player :
	public Actor
{
private:
	InputManager* m_input;
	SceneNode* m_camera;

	UIManager* m_uiManager;
	std::string m_uiMessage;
	int m_coinCount, m_redCoinCount;
public:
	Player(float useGravity, InputManager* input, SceneNode* camera);
	~Player();

	void Start();
	void Update();
	void OnCollision(SceneNode* other);
};

