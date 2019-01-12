#include "UIManager.h"



UIManager::UIManager(Text2D * text)
{
	m_name = "UI Manager";
	m_2DText = text;
	m_text = "New text";
	m_x = -1.0f;
	m_y = 1.0f;
	m_size = 0.1f;
}

UIManager::~UIManager()
{
}

void UIManager::Update()
{
	Component::Update();
	m_2DText->AddText(m_text, m_x, m_y, m_size);
	m_2DText->RenderText();
}

void UIManager::ChangeText(string text)
{
	m_text = text;
}

void UIManager::ChangePosition(float x, float y)
{
	m_x = x;
	m_y = y;
}

void UIManager::ChangeSize(float size)
{
	m_size = size;
}
