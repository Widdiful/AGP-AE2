#pragma once
#include "Actor.h"
#include "InputManager.h"
class Player :
	public Actor
{
private:
	InputManager* m_input;
	float m_jumpVelocity;
public:
	Player(float useGravity, InputManager* input);
	~Player();

	void Start();
	void Update();
};

