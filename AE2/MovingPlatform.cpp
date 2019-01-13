#include "MovingPlatform.h"
#include "SceneNode.h"
#include "Time.h"
#include "Level.h"



MovingPlatform::MovingPlatform()
{
	m_name = "Moving Platform";
	m_moveSpeed = 0.001f;
	m_distanceToChange = 1.0f;
}


MovingPlatform::~MovingPlatform()
{
}

void MovingPlatform::Start()
{
	Component::Start();
	if (m_points.size() == 0)
		m_points.push_back(Vector3(m_node->GetXPos(), m_node->GetYPos(), m_node->GetZPos()));

	m_currentPoint = m_points[m_currentPointIndex];
	m_node->SetIsMovingPlatform(true);
}

void MovingPlatform::Update()
{
	if (m_rootNode && m_rootNode->GetLevel() && m_rootNode->GetLevel()->IsUsingChestCamera()) {
		return;
	}

	Vector3 pos = Vector3(m_node->GetXPos(), m_node->GetYPos(), m_node->GetZPos());

	float dx = pos.x - m_currentPoint.x;
	float dy = pos.y - m_currentPoint.y;
	float dz = pos.z - m_currentPoint.z;

	// check distance to next point
	float distance = sqrt(dx*dx + dy*dy + dz * dz);

	if (distance <= m_distanceToChange) {
		m_currentPoint = GetNextPoint();
	}

	// Calculate how far platform will move in target direction
	float old_x, old_y, old_z;
	old_x = pos.x;
	old_y = pos.y;
	old_z = pos.z;
	float nx, ny, nz;

	nx = (old_x + (m_moveSpeed * (m_currentPoint.x - old_x)));
	ny = (old_y + (m_moveSpeed * (m_currentPoint.y - old_y)));
	nz = (old_z + (m_moveSpeed * (m_currentPoint.z - old_z)));

	m_movementX = nx - old_x;
	m_movementY = ny - old_y;
	m_movementZ = nz - old_z;

	// Move platform
	m_node->AddXPos(m_movementX * Time::getInstance().deltaTime);
	m_node->AddYPos(m_movementY * Time::getInstance().deltaTime);
	m_node->AddZPos(m_movementZ * Time::getInstance().deltaTime);

	Component::Update();
}

Vector3 MovingPlatform::GetNextPoint()
{
	m_currentPointIndex = (m_currentPointIndex + 1) % m_points.size();

	return m_points[m_currentPointIndex];
}

void MovingPlatform::AddPoint(Vector3 newPoint)
{
	m_points.push_back(newPoint);
}

void MovingPlatform::AddPoints(std::vector<Vector3> newPoints)
{
	for (int i = 0; i < newPoints.size(); i++) {
		m_points.push_back(newPoints[i]);
	}
}

Vector3 MovingPlatform::GetMovementInfo()
{
	return Vector3(m_movementX, m_movementY, m_movementZ);
}
