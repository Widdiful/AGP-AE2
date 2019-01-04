#include "Component.h"
#include "scene_node.h"



Component::Component()
{

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

void Component::SetNode(scene_node * node)
{
	m_node = node;
}
