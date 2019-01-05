#include "Component.h"
#include "scene_node.h"



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

void Component::OnCollision(Component* other)
{

}

void Component::SetNode(scene_node * node)
{
	m_node = node;
}

std::string Component::GetName()
{
	return m_name;
}
