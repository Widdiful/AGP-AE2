#pragma once
#include <string>
class scene_node;
class Component
{
protected:
	scene_node* m_node;
	scene_node* m_rootNode;
	std::string m_name;
public:
	Component();
	~Component();

	virtual void Start();
	virtual void Update();
	virtual void OnCollision(Component* other);

	void SetNode(scene_node* node);
	std::string GetName();
};

