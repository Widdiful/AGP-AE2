#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

#include <iostream>
using namespace std;

#include "Camera.h"
#include "text2D.h"
#include "Model.h"
#include "InputManager.h"
#include "ParticleGenerator.h"
#include "Component.h"

class scene_node
{
private:
	Model* m_pModel;
	vector<scene_node*> m_children;
	vector<Component*> m_components;
	scene_node* m_parent;

	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_xScale, m_yScale, m_zScale;
	float m_worldCentreX, m_worldCentreY, m_worldCentreZ;
	float m_worldScale;

	bool m_collisionEnabled = true;

public:
	scene_node();
	~scene_node();
	void SetModel(Model* model);
	void addChildNode(scene_node* n);
	bool detatchNode(scene_node* n);
	void SetParent(scene_node* n);
	scene_node* GetParent();
	void Update(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	void LookAt_XZ(float x, float z);
	void MoveForward(float distance);
	bool MoveForward(float distance, scene_node* rootNode);
	bool CheckCollision(scene_node* compareTree);
	bool CheckCollision(scene_node* compareTree, scene_node* objectTreeRoot);
	XMVECTOR GetWorldCentrePosition();
	void UpdateCollisionTree(XMMATRIX* world, float scale);
	void AddComponent(Component* component);
	scene_node* GetRootNode();

	// POSITION INFO

	void SetXPos(float x);
	void SetYPos(float y);
	void SetZPos(float z);
	void SetPos(float x, float y, float z);

	void AddXPos(float x);
	void AddYPos(float y);
	void AddZPos(float z);
	void AddPos(float x, float y, float z);

	bool AddXPos(float x, scene_node* rootNode);
	bool AddYPos(float y, scene_node* rootNode);
	bool AddZPos(float z, scene_node* rootNode);
	bool AddPos(float x, float y, float z, scene_node* rootNode);

	float GetXPos();
	float GetYPos();
	float GetZPos();

	// ROTATION INFO

	void SetXRot(float x);
	void SetYRot(float y);
	void SetZRot(float z);
	void SetRot(float x, float y, float z);

	void AddXRot(float x);
	void AddYRot(float y);
	void AddZRot(float z);
	void AddRot(float x, float y, float z);

	float GetXRot();
	float GetYRot();
	float GetZRot();

	// SCALE INFO

	void SetXScale(float x);
	void SetYScale(float y);
	void SetZScale(float z);
	void SetScale(float x, float y, float z);

	void AddXScale(float x);
	void AddYScale(float y);
	void AddZScale(float z);
	void AddScale(float x, float y, float z);

	float GetXScale();
	float GetYScale();
	float GetZScale();
};

