#pragma once
#include "Component.h"
class Actor :
	public Component
{
protected:
	// Physics and movement
	bool m_useGravity;
	float m_velocityX, m_velocityY, m_velocityZ;
	float m_gravity, m_gravityMax;
	float m_jumpVelocity, m_moveSpeed;
	bool m_grounded;
	bool m_moving = false;

	// Health
	float m_iframes;
	bool m_visible;
	int m_health;
	float m_deathY;

public:
	Actor(float useGravity);
	~Actor();

	virtual void Start();
	virtual void Update();
	virtual void TakeDamage();
	virtual void OnCollision(SceneNode* other);
};

