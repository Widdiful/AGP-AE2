#include "Component.h"
#include "SceneNode.h"



Component::Component()
{
	m_name = "";
}

Component::~Component()
{
}

void Component::Start()
{
	// Code ran after creation once every component has been created
	// Put references to other components here
	if (m_node) {
		m_rootNode = m_node->GetRootNode();
		if (m_rootNode != m_node)
			m_level = m_rootNode->GetLevel();
	}
}

void Component::Update()
{
	// Code ran on every frame
}

void Component::OnCollision(SceneNode* other)
{
	// Code ran whenever node touches another
}

void Component::SetNode(SceneNode * node)
{
	m_node = node;
}

std::string Component::GetName()
{
	return m_name;
}
