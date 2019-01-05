#pragma once
#include "Actor.h"
#include "InputManager.h"
#include "Camera.h"
class Player :
	public Actor
{
private:
	InputManager* m_input;
	scene_node* m_camera;
public:
	Player(float useGravity, InputManager* input, scene_node* camera);
	~Player();

	void Start();
	void Update();
};

