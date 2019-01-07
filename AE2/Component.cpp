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
	if (m_node) m_rootNode = m_node->GetRootNode();
}

void Component::Update()
{
}

void Component::OnCollision(SceneNode* other)
{

}

void Component::SetNode(SceneNode * node)
{
	m_node = node;
}

std::string Component::GetName()
{
	return m_name;
}
