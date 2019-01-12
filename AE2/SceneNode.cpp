#include "SceneNode.h"
#include "Maths.h"
#include "Particles.h"



SceneNode::SceneNode(string name)
{
	m_pModel = NULL;
	m_name = name;
	m_position = Vector3(0, 0, 0);
	m_rotation = Vector3(0, 0, 0);
	m_scale = Vector3(1, 1, 1);

	//m_collisionType = Cube;
}


SceneNode::~SceneNode()
{
	delete m_pModel;
	m_pModel = nullptr;
	for (int i = 0; i < m_components.size(); i++) {
		delete m_components[i];
	}
	m_components.clear();
}

void SceneNode::SetModel(Model * model)
{
	m_pModel = model;
}

void SceneNode::SetModel(char* model, char* texture, ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_pModel = new Model(device, context);
	m_pModel->LoadObjModel(model);
	m_pModel->AddTexture(texture);
}

void SceneNode::addChildNode(SceneNode * n)
{
	m_children.push_back(n);
	n->SetParent(this);
}

void SceneNode::removeChildNode(SceneNode * n)
{
	m_children.erase(std::remove(m_children.begin(), m_children.end(), n), m_children.end());
}

bool SceneNode::detatchNode(SceneNode * n)
{
	// traverse tree to find node to detatch
	for (int i = 0; i < m_children.size(); i++)
	{
		if (n == m_children[i])
		{
			m_children.erase(m_children.begin() + i);
			return true;
		}
		if (m_children[i]->detatchNode(n) == true) return true;
	}
	return false; // node not in this tree

}

void SceneNode::SetParent(SceneNode * n)
{
	m_parent = n;
}

void SceneNode::SetLevel(Level * n)
{
	m_level = n;
}

SceneNode * SceneNode::GetParent()
{
	return m_parent;
}

void SceneNode::Update(XMMATRIX * world, XMMATRIX * view, XMMATRIX * projection)
{
	if (!m_enabled) return;

	// Only draw object if visible
	if (m_visible) {
		// the local_world matrix will be used to calc the local transformations for this node
		m_localWorldMatrix = XMMatrixIdentity();

		m_localWorldMatrix = XMMatrixRotationX(XMConvertToRadians(m_rotation.x));
		m_localWorldMatrix *= XMMatrixRotationY(XMConvertToRadians(m_rotation.y));
		m_localWorldMatrix *= XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));

		m_localWorldMatrix *= XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

		m_localWorldMatrix *= XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		// the local matrix is multiplied by the passed in world matrix that contains the concatenated
		// transformations of all parent nodes so that this nodes transformations are relative to those
		m_localWorldMatrix *= *world;

		// only draw if there is a model attached
		if (m_pModel) m_pModel->Draw(&m_localWorldMatrix, view, projection);
	}
	// run all component update functions
	for (int i = 0; i < m_components.size(); i++) {
		m_components[i]->Update();
	}

	// traverse all child nodes, passing in the concatenated world matrix
	for (int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->Update(&m_localWorldMatrix, view, projection);
	}

}

void SceneNode::LookAt_XZ(float x, float z)
{
	float dx, dz;

	dx = x - m_position.x;
	dz = z - m_position.z;

	m_rotation.y = atan2(dx, dz) * (180.0 / XM_PI);
}

void SceneNode::LerpAt_XZ(float x, float z, float lerpSpeed)
{
	float dx = x - m_position.x;
	float dz = z - m_position.z;

	float targetRotation = atan2(dx, dz) * (180.0 / XM_PI);

	if (targetRotation - m_rotation.y >= 180) {
		targetRotation -= 360;
	}
	m_rotation.y = (m_rotation.y + (lerpSpeed * (targetRotation - m_rotation.y)));
}

void SceneNode::LookAt_XYZ(float x, float y, float z)
{
	float dx, dy, dz;

	dx = x - m_position.x;
	dy = y - m_position.y;
	dz = z - m_position.z;

	m_rotation.x = dy;
	m_rotation.y = atan2(dx, dz) * (180.0 / XM_PI);
}

void SceneNode::LookAt_XZ(float x, float z, float rot)
{
	float dx, dz;

	dx = x - m_position.x;
	dz = z - m_position.z;

	m_rotation.y = atan2(dx, dz) * (180.0 / XM_PI) + rot;
}

void SceneNode::MoveForward(float distance)
{
	m_position.x += sin(m_rotation.y * (XM_PI / 180.0)) * distance;
	m_position.z += cos(m_rotation.y * (XM_PI / 180.0)) * distance;
}

bool SceneNode::MoveForward(float distance, SceneNode * rootNode)
{
	float old_x = m_position.x;
	float old_z = m_position.z;

	m_position.x += sin(m_rotation.y * (XM_PI / 180.0)) * distance;
	m_position.z += cos(m_rotation.y * (XM_PI / 180.0)) * distance;

	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true && !m_isMovingPlatform)
	{
		// if collision restore state
		m_position.x = old_x;
		m_position.z = old_z;

		return true;
	}
}

bool SceneNode::CheckCollision(SceneNode * compareTree)
{
	return CheckCollision(compareTree, this);
}

bool SceneNode::CheckCollision(SceneNode * compareTree, SceneNode * objectTreeRoot)
{
	// check to see if root of tree being compared is same as root node of object tree being checked
	// i.e. stop object node and children being checked against each other
	if (objectTreeRoot == compareTree) return false;
	if (!(compareTree->m_enabled && objectTreeRoot->m_enabled)) return false;

	// only check for collisions if both nodes contain a model
	if (m_pModel && compareTree->m_pModel)
	{
		XMVECTOR v1 = GetWorldCentrePosition();
		XMVECTOR v2 = compareTree->GetWorldCentrePosition();
		XMVECTOR vdiff = v1 - v2;

		//XMVECTOR a = XMVector3Length(vdiff);
		float x1 = XMVectorGetX(v1);
		float x2 = XMVectorGetX(v2);
		float y1 = XMVectorGetY(v1);
		float y2 = XMVectorGetY(v2);
		float z1 = XMVectorGetZ(v1);
		float z2 = XMVectorGetZ(v2);

		bool collision = false;

		if (m_collisionType == compareTree->m_collisionType) {
			switch (m_collisionType) {
			case Sphere: {
				float dx = x1 - x2;
				float dy = y1 - y2;
				float dz = z1 - z2;

				// check bounding sphere collision
				if (sqrt(dx*dx + dy * dy + dz * dz) <
					(compareTree->m_pModel->GetBoundingSphereRadius() * compareTree->m_worldScale) +
					(this->m_pModel->GetBoundingSphereRadius() * m_worldScale))
				{
					collision = true;
				}
			}
						 break;

			case Cube: {
				float xbounds1 = this->m_pModel->GetCubeBounds().x * m_scale.x;
				float ybounds1 = this->m_pModel->GetCubeBounds().y * m_scale.y;
				float zbounds1 = this->m_pModel->GetCubeBounds().z * m_scale.z;
				float xbounds2 = compareTree->m_pModel->GetCubeBounds().x * compareTree->m_scale.x;
				float ybounds2 = compareTree->m_pModel->GetCubeBounds().y * compareTree->m_scale.y;
				float zbounds2 = compareTree->m_pModel->GetCubeBounds().z * compareTree->m_scale.z;
				bool xCol = (x1 - xbounds1 < x2 + xbounds2) && (x1 + xbounds1 > x2 - xbounds2);
				bool yCol = (y1 - ybounds1 < y2 + ybounds2) && (y1 + ybounds1 > y2 - ybounds2);
				bool zCol = (z1 - zbounds1 < z2 + zbounds2) && (z1 + zbounds1 > z2 - zbounds2);

				if (xCol && yCol && zCol)
					collision = true;
			}
					   break;
			}
		}
		else {
			float cubeX, cubeY, cubeZ;
			float sphereX, sphereY, sphereZ;

			float xbounds;
			float ybounds;
			float zbounds;

			switch (m_collisionType) {
			case Sphere:
				cubeX = XMVectorGetX(compareTree->GetWorldCentrePosition());
				cubeY = XMVectorGetY(compareTree->GetWorldCentrePosition());
				cubeZ = XMVectorGetZ(compareTree->GetWorldCentrePosition());
				sphereX = XMVectorGetX(this->GetWorldCentrePosition());
				sphereY = XMVectorGetY(this->GetWorldCentrePosition());
				sphereZ = XMVectorGetZ(this->GetWorldCentrePosition());
				xbounds = compareTree->m_pModel->GetCubeBounds().x * compareTree->m_scale.x;
				ybounds = compareTree->m_pModel->GetCubeBounds().y * compareTree->m_scale.y;
				zbounds = compareTree->m_pModel->GetCubeBounds().z * compareTree->m_scale.z;
				break;
			case Cube:
				cubeX = XMVectorGetX(this->GetWorldCentrePosition());
				cubeY = XMVectorGetY(this->GetWorldCentrePosition());
				cubeZ = XMVectorGetZ(this->GetWorldCentrePosition());
				sphereX = XMVectorGetX(compareTree->GetWorldCentrePosition());
				sphereY = XMVectorGetY(compareTree->GetWorldCentrePosition());
				sphereZ = XMVectorGetZ(compareTree->GetWorldCentrePosition());
				xbounds = this->m_pModel->GetCubeBounds().x * this->m_scale.x;
				ybounds = this->m_pModel->GetCubeBounds().y * this->m_scale.y;
				zbounds = this->m_pModel->GetCubeBounds().z * this->m_scale.z;
				break;
			}

			float step = m_pModel->GetBoundingSphereRadius();
			float x, y, z;

			x = Maths::clamp(sphereX, cubeX - xbounds, cubeX + xbounds);
			y = Maths::clamp(sphereY, cubeY - ybounds, cubeY + ybounds);
			z = Maths::clamp(sphereZ, cubeZ - zbounds, cubeZ + zbounds);

			Vector3 closestPoint = Vector3(x, y, z);

			float dx = sphereX - closestPoint.x;
			float dy = sphereY - closestPoint.y;
			float dz = sphereZ - closestPoint.z;

			// check bounding sphere collision
			if (sqrt(dx*dx + dy * dy + dz * dz) < this->m_pModel->GetBoundingSphereRadius() * m_worldScale) {
				collision = true;
			}
		}

		if (collision) {
			for (int i = 0; i < m_components.size(); i++) {
				m_components[i]->OnCollision(compareTree);
			}
			for (int i = 0; i < compareTree->m_components.size(); i++) {
				compareTree->m_components[i]->OnCollision(this);
			}
			if (!(m_collisionEnabled && compareTree->m_collisionEnabled)) return false;
			return true;
		}
		
	}

	// iterate through compared tree child nodes
	for (int i = 0; i < compareTree->m_children.size(); i++)
	{
		// check for collsion against all compared tree child nodes 
		if (CheckCollision(compareTree->m_children[i], objectTreeRoot) == true) return true;
	}

	// iterate through composite object child nodes
	for (int i = 0; i < m_children.size(); i++)
	{
		// check all the child nodes of the composite object against compared tree
		if (m_children[i]->CheckCollision(compareTree, objectTreeRoot) == true) return true;
	}

	return false;

}

XMVECTOR SceneNode::GetWorldCentrePosition()
{
	return XMVectorSet(m_worldCentreX, m_worldCentreY, m_worldCentreZ, 0.0);
}

void SceneNode::UpdateCollisionTree(XMMATRIX * world, float scale)
{
	// the local_world matrix will be used to calculate the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixRotationX(XMConvertToRadians(m_rotation.x));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_rotation.y));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));

	local_world *= XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	local_world *= XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	// calc the world space scale of this object, is needed to calculate the  
	// correct bounding sphere radius of an object in a scaled hierarchy
	m_worldScale = scale * m_scale.x;

	XMVECTOR v;
	if (/*m_pModel*/ false)
	{
		v = XMVectorSet(m_pModel->GetBoundingSphereWorldSpacePosition().x,
			m_pModel->GetBoundingSphereWorldSpacePosition().y,
			m_pModel->GetBoundingSphereWorldSpacePosition().z, 0.0);
	}
	else v = XMVectorSet(0, 0, 0, 0); // no model, default to 0

	// find and store world space bounding sphere centre
	v = XMVector3Transform(v, local_world);
	m_worldCentreX = XMVectorGetX(v);
	m_worldCentreY = XMVectorGetY(v);
	m_worldCentreZ = XMVectorGetZ(v);

	// traverse all child nodes, passing in the concatenated world matrix and scale
	for (int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->UpdateCollisionTree(&local_world, m_worldScale);
	}

}

void SceneNode::AddComponent(Component * component)
{
	m_components.push_back(component);
	component->SetNode(this);
	//component->Start();
}

Component * SceneNode::GetComponent(std::string name)
{
	for (int i = 0; i < m_components.size(); i++) {
		if (m_components[i]->GetName() == name) {
			return m_components[i];
		}
	}
	return nullptr;
}

Component * SceneNode::GetComponentInChildren(std::string name)
{
	for (int i = 0; i < m_components.size(); i++) {
		if (m_components[i]->GetName() == name) {
			return m_components[i];
		}
	}
	for (int i = 0; i < m_children.size(); i++) {
		Component* temp = m_children[i]->GetComponentInChildren(name);
		if (temp != nullptr) return temp;
	}
	return nullptr;
}

SceneNode * SceneNode::FindNode(string name)
{
	for (int i = 0; i < m_children.size(); i++) {
		if (m_children[i]->GetName() == name) {
			return m_children[i];
		}
	}
	for (int i = 0; i < m_children.size(); i++) {
		SceneNode* temp = m_children[i]->FindNode(name);
		if (temp != nullptr) return temp;
	}
	return nullptr;
}

string SceneNode::GetName()
{
	return m_name;
}

void SceneNode::StartComponents()
{
	for (int i = 0; i < m_components.size(); i++) {
		m_components[i]->Start();
	}
	for (int i = 0; i < m_children.size(); i++) {
		m_children[i]->StartComponents();
	}
}

void SceneNode::DeleteComponents()
{
	for (int i = 0; i < m_components.size(); i++) {
		delete m_components[i];
		m_components[i] = nullptr;
	}
	for (int i = 0; i < m_children.size(); i++) {
		m_children[i]->DeleteComponents();
	}
}

void SceneNode::SetEnabled(bool val)
{
	m_enabled = val;
}

bool SceneNode::GetEnabled()
{
	return m_enabled;
}

void SceneNode::SetVisible(bool val)
{
	m_visible = val;
}

bool SceneNode::GetVisible()
{
	return m_visible;
}

void SceneNode::SetIsMovingPlatform(bool val)
{
	m_isMovingPlatform = val;
}

Model * SceneNode::GetModel()
{
	return m_pModel;
}

void SceneNode::SetCollision(bool val)
{
	m_collisionEnabled = val;
}

void SceneNode::SetCollisionType(CollisionType val)
{
	m_collisionType = val;
}

SceneNode * SceneNode::GetRootNode()
{
	if (m_parent) {
		return m_parent->GetRootNode();
	}
	return this;
}

Level * SceneNode::GetLevel()
{
	return m_level;
}

void SceneNode::SetXPos(float x)
{
	m_position.x = x;
}

void SceneNode::SetYPos(float y)
{
	m_position.y = y;
}

void SceneNode::SetZPos(float z)
{
	m_position.z = z;
}

void SceneNode::SetPos(float x, float y, float z)
{
	SetXPos(x);
	SetYPos(y);
	SetZPos(z);
}

void SceneNode::SetXRot(float x)
{
	m_rotation.x = x;
}

void SceneNode::SetYRot(float y)
{
	m_rotation.y = y;
	if (m_rotation.y > 360) m_rotation.y -= 360;
	if (m_rotation.y < -360) m_rotation.y += 360;
}

void SceneNode::SetZRot(float z)
{
	m_rotation.z = z;
}

void SceneNode::SetRot(float x, float y, float z)
{
	SetXRot(x);
	SetYRot(y);
	SetZRot(z);
}

void SceneNode::SetXScale(float x)
{
	m_scale.x = x;
}

void SceneNode::SetYScale(float y)
{
	m_scale.y = y;
}

void SceneNode::SetZScale(float z)
{
	m_scale.z = z;
}

void SceneNode::SetScale(float x, float y, float z)
{
	SetXScale(x);
	SetYScale(y);
	SetZScale(z);
}

void SceneNode::AddXPos(float x)
{
	m_position.x += x;
}

void SceneNode::AddYPos(float y)
{
	m_position.y += y;
}

void SceneNode::AddZPos(float z)
{
	m_position.z += z;
}

void SceneNode::AddPos(float x, float y, float z)
{
	AddXPos(x);
	AddYPos(y);
	AddZPos(z);
}

bool SceneNode::AddXPos(float x, SceneNode * rootNode)
{
	float old_x = m_position.x;	// save current state 
	m_position.x += x;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_position.x = old_x;

		return true;
	}

	return false;
}

bool SceneNode::AddYPos(float y, SceneNode * rootNode)
{
	float old_y = m_position.y;	// save current state 
	m_position.y += y;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_position.y = old_y;

		return true;
	}

	return false;
}

bool SceneNode::AddZPos(float z, SceneNode * rootNode)
{
	float old_z = m_position.y;	// save current state 
	m_position.z += z;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_position.z = old_z;

		return true;
	}

	return false;
}

bool SceneNode::AddPos(float x, float y, float z, SceneNode * rootNode)
{
	if (!(AddXPos(x, rootNode) && AddYPos(y, rootNode) && AddZPos(z, rootNode)))
		return true;
	else
		return false;
}

void SceneNode::AddXRot(float x)
{
	m_rotation.x += x;
}

void SceneNode::AddYRot(float y)
{
	m_rotation.y += y;
}

void SceneNode::AddZRot(float z)
{
	m_rotation.z += z;
}

void SceneNode::AddRot(float x, float y, float z)
{
	AddXRot(x);
	AddYRot(y);
	AddZRot(z);
}

void SceneNode::AddXScale(float x)
{
	m_scale.x += x;
}

void SceneNode::AddYScale(float y)
{
	m_scale.y += y;
}

void SceneNode::AddZScale(float z)
{
	m_scale.z += z;
}

void SceneNode::AddScale(float x, float y, float z)
{
	AddXScale(x);
	AddYScale(y);
	AddZScale(z);
}

float SceneNode::GetXPos()
{
	return m_position.x;
}

float SceneNode::GetYPos()
{
	return m_position.y;
}

float SceneNode::GetZPos()
{
	return m_position.z;
}

float SceneNode::GetWorldXPos()
{
	return m_worldCentreX;
}

float SceneNode::GetWorldYPos()
{
	return m_worldCentreY;
}

float SceneNode::GetWorldZPos()
{
	return m_worldCentreZ;
}

float SceneNode::GetXRot()
{
	return m_rotation.x;
}

float SceneNode::GetYRot()
{
	return m_rotation.y;
}

float SceneNode::GetZRot()
{
	return m_rotation.z;
}

float SceneNode::GetXScale()
{
	return m_scale.x;
}

float SceneNode::GetYScale()
{
	return m_scale.y;
}

float SceneNode::GetZScale()
{
	return m_scale.z;
}