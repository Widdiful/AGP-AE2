#pragma once
#include "Component.h"
class Pickup :
	public Component
{
private:
	float m_rotateSpeed;
public:
	Pickup();
	~Pickup();

	void Start();
	void Update();
};

