#include "Enemy.h"
#include "SceneNode.h"
#include "Level.h"


Enemy::Enemy(bool gravity) : Actor(gravity)
{
	m_name = "Enemy";
	m_jumpVelocity = 0.05f;
	m_moveSpeed = 0.025f;
	m_distanceToChange = 0.1f;
	m_chaseDistance = 50;

	m_points.push_back(Vector3(0, 0, 0));
	m_points.push_back(Vector3(10, 0, 0));
	m_points.push_back(Vector3(0, 0, 10));
	m_points.push_back(Vector3(10, 10, 10));
	m_points.push_back(Vector3(20, 10, 0));
	m_points.push_back(Vector3(-20, 10, 20));

	m_currentPoint = m_points[m_currentPointIndex];
}


Enemy::~Enemy()
{
}

void Enemy::Start()
{
	Actor::Start();
	m_player = m_rootNode->FindNode("Player");
}

void Enemy::Update()
{
	// check distance to player
	Vector3 pos = Vector3(m_node->GetXPos(), m_node->GetYPos(), m_node->GetZPos());
	float dx = pos.x - m_player->GetXPos();
	float dz = pos.z - m_player->GetZPos();

	float distance = sqrt(dx*dx + dz * dz);
	Vector3 target;

	// Chase player if close enough
	if (distance <= m_chaseDistance) {
		target = Vector3(m_player->GetXPos(), m_player->GetZPos(), m_player->GetZPos());
	}

	// Follow path
	else {
		target = m_currentPoint;

		dx = pos.x - m_currentPoint.x;
		dz = pos.z - m_currentPoint.z;

		// check distance to next point
		distance = sqrt(dx*dx + dz * dz);

		if (distance <= m_distanceToChange) {
			m_currentPoint = GetNextPoint();
		}

	}

	// Look at and move towards player or path point
	m_node->LookAt_XZ(target.x, target.z);
	m_node->MoveForward(Time::getInstance().deltaTime * m_moveSpeed, m_node->GetRootNode());

	// Jump
	if (target.y > m_node->GetYPos() && m_grounded) {
		m_velocityY += m_jumpVelocity;
	}

	Actor::Update();
}

Vector3 Enemy::GetNextPoint()
{
	m_currentPointIndex = (m_currentPointIndex + 1) % m_points.size();

	return m_points[m_currentPointIndex];
}

void Enemy::AddPoint(Vector3 newPoint)
{
	m_points.push_back(newPoint);
}

void Enemy::AddPoints(vector<Vector3> newPoints)
{
	for (int i = 0; i < newPoints.size(); i++) {
		m_points.push_back(newPoints[i]);
	}
}
