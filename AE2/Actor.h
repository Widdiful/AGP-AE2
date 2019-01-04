#pragma once
#include "Component.h"
class Actor :
	public Component
{
private:
	bool m_useGravity = true;
	float m_velocityX, m_velocityY, m_velocityZ;
	float m_gravity;
	float m_gravityMax;
public:
	Actor();
	~Actor();

	void Start();
	void Update();
};

