#pragma once
#include "Component.h"
#include "ParticleGenerator.h"
class Particles :
	public Component
{
private:
	ParticleGenerator* m_particles;
public:
	Particles(ParticleGenerator::particleTypes type, ID3D11Device* device, ID3D11DeviceContext* context);
	~Particles();
	void Start();
	void Update(XMMATRIX* view, XMMATRIX* projection, XMFLOAT3 camera);
	void Play();
	void Stop();
	void UpdatePosition(float x, float y, float z);
};

