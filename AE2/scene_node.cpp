#include "scene_node.h"



scene_node::scene_node()
{
	m_pModel = NULL;
	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;
	m_xAngle = 0.0f;
	m_xAngle = 0.0f;
	m_xAngle = 0.0f;
	m_xScale = 1.0f;
	m_yScale = 1.0f;
	m_zScale = 1.0f;
}


scene_node::~scene_node()
{
}

void scene_node::SetModel(Model * model)
{
	m_pModel = model;
}

void scene_node::addChildNode(scene_node * n)
{
	m_children.push_back(n);
}

bool scene_node::detatchNode(scene_node * n)
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

void scene_node::execute(XMMATRIX * world, XMMATRIX * view, XMMATRIX * projection)
{
	// the local_world matrix will be used to calc the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));

	local_world *= XMMatrixScaling(m_xScale, m_yScale, m_zScale);

	local_world *= XMMatrixTranslation(m_x, m_y, m_z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	// only draw if there is a model attached
	if (m_pModel) m_pModel->Draw(&local_world, view, projection);

	// traverse all child nodes, passing in the concatenated world matrix
	for (int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->execute(&local_world, view, projection);
	}

}

void scene_node::LookAt_XZ(float x, float z)
{
	float dx, dz;

	dx = x - m_x;
	dz = z - m_z;

	m_yAngle = atan2(dx, dz) * (180.0 / XM_PI);
}

void scene_node::MoveForward(float distance)
{
	m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance;
	m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance;
}

bool scene_node::MoveForward(float distance, scene_node * rootNode)
{
	float old_x = m_x;
	float old_z = m_z;

	m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance;
	m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance;

	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_x = old_x;
		m_z = old_z;

		return true;
	}
}

bool scene_node::CheckCollision(scene_node * compareTree)
{
	return CheckCollision(compareTree, this);
}

bool scene_node::CheckCollision(scene_node * compareTree, scene_node * objectTreeRoot)
{
	// check to see if root of tree being compared is same as root node of object tree being checked
	// i.e. stop object node and children being checked against each other
	if (objectTreeRoot == compareTree) return false;

	// only check for collisions if both nodes contain a model
	if (m_pModel && compareTree->m_pModel && m_collisionEnabled && compareTree->m_collisionEnabled)
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

		float dx = x1 - x2;
		float dy = y1 - y2;
		float dz = z1 - z2;

		// check bounding sphere collision
		if (sqrt(dx*dx + dy * dy + dz * dz) <
			(compareTree->m_pModel->GetBoundingSphereRadius() * compareTree->m_worldScale) +
			(this->m_pModel->GetBoundingSphereRadius() * m_worldScale))
		{
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

XMVECTOR scene_node::GetWorldCentrePosition()
{
	return XMVectorSet(m_worldCentreX, m_worldCentreY, m_worldCentreZ, 0.0);
}

void scene_node::UpdateCollisionTree(XMMATRIX * world, float scale)
{
	// the local_world matrix will be used to calculate the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));

	local_world *= XMMatrixScaling(m_xScale, m_yScale, m_zScale);

	local_world *= XMMatrixTranslation(m_x, m_y, m_z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	// calc the world space scale of this object, is needed to calculate the  
	// correct bounding sphere radius of an object in a scaled hierarchy
	m_worldScale = scale * m_xScale;

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

void scene_node::SetXPos(float x)
{
	m_x = x;
}

void scene_node::SetYPos(float y)
{
	m_y = y;
}

void scene_node::SetZPos(float z)
{
	m_z = z;
}

void scene_node::SetPos(float x, float y, float z)
{
	SetXPos(x);
	SetYPos(y);
	SetZPos(z);
}

void scene_node::SetXRot(float x)
{
	m_xAngle = x;
}

void scene_node::SetYRot(float y)
{
	m_yAngle = y;
}

void scene_node::SetZRot(float z)
{
	m_zAngle = z;
}

void scene_node::SetRot(float x, float y, float z)
{
	SetXRot(x);
	SetYRot(y);
	SetZRot(z);
}

void scene_node::SetXScale(float x)
{
	m_xScale = x;
}

void scene_node::SetYScale(float y)
{
	m_yScale = y;
}

void scene_node::SetZScale(float z)
{
	m_zScale = z;
}

void scene_node::SetScale(float x, float y, float z)
{
	SetXScale(x);
	SetYScale(y);
	SetZScale(z);
}

void scene_node::AddXPos(float x)
{
	m_x += x;
}

void scene_node::AddYPos(float y)
{
	m_y += y;
}

void scene_node::AddZPos(float z)
{
	m_z += z;
}

void scene_node::AddPos(float x, float y, float z)
{
	AddXPos(x);
	AddYPos(y);
	AddZPos(z);
}

bool scene_node::AddXPos(float x, scene_node * rootNode)
{
	float old_x = m_x;	// save current state 
	m_x += x;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_x = old_x;

		return true;
	}

	return false;
}

bool scene_node::AddYPos(float y, scene_node * rootNode)
{
	float old_y = m_y;	// save current state 
	m_y += y;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_y = old_y;

		return true;
	}

	return false;
}

bool scene_node::AddZPos(float z, scene_node * rootNode)
{
	float old_z = m_z;	// save current state 
	m_z += z;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_z = old_z;

		return true;
	}

	return false;
}

bool scene_node::AddPos(float x, float y, float z, scene_node * rootNode)
{
	if (AddXPos(x, rootNode) && AddYPos(y, rootNode) && AddZPos(z, rootNode))
		return true;
	else
		return false;
}

void scene_node::AddXRot(float x)
{
	m_xAngle += x;
}

void scene_node::AddYRot(float y)
{
	m_yAngle += y;
}

void scene_node::AddZRot(float z)
{
	m_zAngle += z;
}

void scene_node::AddRot(float x, float y, float z)
{
	AddXRot(x);
	AddYRot(y);
	AddZRot(z);
}

void scene_node::AddXScale(float x)
{
	m_xScale += x;
}

void scene_node::AddYScale(float y)
{
	m_yScale += y;
}

void scene_node::AddZScale(float z)
{
	m_zScale += z;
}

void scene_node::AddScale(float x, float y, float z)
{
	AddXScale(x);
	AddYScale(y);
	AddZScale(z);
}

float scene_node::GetXPos()
{
	return m_x;
}

float scene_node::GetYPos()
{
	return m_y;
}

float scene_node::GetZPos()
{
	return m_z;
}

float scene_node::GetXRot()
{
	return m_xAngle;
}

float scene_node::GetYRot()
{
	return m_yAngle;
}

float scene_node::GetZRot()
{
	return m_zAngle;
}

float scene_node::GetXScale()
{
	return m_xScale;
}

float scene_node::GetYScale()
{
	return m_yScale;
}

float scene_node::GetZScale()
{
	return m_zScale;
}