#pragma once
#include "Component.h"
class Actor :
	public Component
{
protected:
	bool m_useGravity;
	float m_velocityX, m_velocityY, m_velocityZ;
	float m_gravity;
	float m_gravityMax;
public:
	Actor(float useGravity);
	~Actor();

	virtual void Start();
	virtual void Update();
};

