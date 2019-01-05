#pragma once
#include "Component.h"
#include "text2D.h"
class UIManager :
	public Component
{
private:
	Text2D* m_2DText;
	string m_text;
	float m_x, m_y, m_size;

public:
	UIManager(Text2D* text);
	~UIManager();

	void Update();
	void ChangeText(string text);
	void ChangePosition(float x, float y);
	void ChangeSize(float size);
};

