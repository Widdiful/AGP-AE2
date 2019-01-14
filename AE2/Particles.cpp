#include "Particles.h"
#include "SceneNode.h"


Particles::Particles(ParticleGenerator::particleTypes type, ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Initialise default variables
	m_name = "Particles";
	m_particles = new ParticleGenerator(device, context);
	m_particles->SetType(type);
	m_particles->CreateParticle();
}


Particles::~Particles()
{
	if (m_particles) delete m_particles;
}

void Particles::Start()
{
	Component::Start();
	m_particles->SetPos(m_node->GetXPos(), m_node->GetYPos(), m_node->GetZPos());
}

void Particles::Update(XMMATRIX* view, XMMATRIX* projection, XMFLOAT3 camera)
{
	if (!m_node->GetEnabled()) return;
	Component::Update();
	if (m_particles) m_particles->Draw(view, projection, &camera);
}

void Particles::Play()
{
	m_particles->Play();
}

void Particles::Stop()
{
	m_particles->SetEnabled(false);
}

void Particles::UpdatePosition(float x, float y, float z)
{
	m_particles->SetPos(x, y, z);
}
