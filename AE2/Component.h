#pragma once
#include <string>
class SceneNode;
class Level;
class Component
{
protected:
	SceneNode* m_node;
	SceneNode* m_rootNode;
	std::string m_name;
	Level* m_level;
public:
	Component();
	~Component();

	virtual void Start();
	virtual void Update();
	virtual void OnCollision(SceneNode* other);

	void SetNode(SceneNode* node);
	std::string GetName();
};

