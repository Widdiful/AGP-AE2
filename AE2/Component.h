#pragma once
class scene_node;
class Component
{
protected:
	scene_node* m_node;
	scene_node* m_rootNode;
public:
	Component();
	~Component();

	virtual void Start();
	virtual void Update();

	void SetNode(scene_node* node);
};

