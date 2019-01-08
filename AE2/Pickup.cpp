#include "Pickup.h"
#include "SceneNode.h"
#include "Level.h"



Pickup::Pickup() : Component()
{
	m_rotateSpeed = -0.3;
}


Pickup::~Pickup()
{
}

void Pickup::Start()
{
	Component::Start();
	m_node->SetCollision(false);
}

void Pickup::Update()
{
	m_node->AddYRot(m_rotateSpeed * m_level->GetDeltaTime());
}
