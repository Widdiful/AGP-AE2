#include "ParticleGenerator.h"
#include <math.h>
#include "Time.h"

struct PARTICLE_CONSTANT_BUFFER {
	XMMATRIX WorldViewProjection;
	XMFLOAT4 color;
};

XMFLOAT3 ParticleGenerator::CalculateModelCentrePoint()
{
	float minX = 100;
	float minY = 100;
	float minZ = 100;
	float maxX = -100;
	float maxY = -100;
	float maxZ = -100;

	for (int i = 0; i < m_pObject->numverts; i++) {
		XMFLOAT3 currentVertex = m_pObject->vertices[i].Pos;

		if (currentVertex.x < minX) minX = currentVertex.x;
		if (currentVertex.x > maxX) maxX = currentVertex.x;

		if (currentVertex.y < minY) minY = currentVertex.y;
		if (currentVertex.y > maxY) maxY = currentVertex.y;

		if (currentVertex.z < minZ) minZ = currentVertex.z;
		if (currentVertex.z > maxZ) maxZ = currentVertex.z;
	}

	m_bounding_sphere_centre_x = (minX + maxX) / 2.0;
	m_bounding_sphere_centre_y = (minY + maxY) / 2.0;
	m_bounding_sphere_centre_z = (minZ + maxZ) / 2.0;

	return XMFLOAT3(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z);
}

float ParticleGenerator::CalculateBoundingSphereRadius()
{
	float maxDistance = 0;
	for (int i = 0; i < m_pObject->numverts; i++) {
		XMFLOAT3 currentVert = m_pObject->vertices[i].Pos;
		float currentDistance = sqrt(pow(m_bounding_sphere_centre_x + currentVert.x, 2) +
			pow(m_bounding_sphere_centre_y + currentVert.y, 2) +
			pow(m_bounding_sphere_centre_z + currentVert.z, 2));

		if (currentDistance > maxDistance) maxDistance = currentDistance;
	}
	m_bounding_sphere_radius = maxDistance;
	return maxDistance;
}

float ParticleGenerator::RandomZeroToOne()
{
	return ((rand() % 1000) / 1000.0f);
}

float ParticleGenerator::RandomNegOneToPosOne()
{
	return ((rand() % 1000) / 1000.0f) - (rand() % 2);
}

ParticleGenerator::ParticleGenerator(ID3D11Device * pD3DDevice, ID3D11DeviceContext * pImmediateContext)
{
	m_pD3DDevice = pD3DDevice;
	m_pImmediateContext = pImmediateContext;

	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;
	m_xAngle = 0.0f;
	m_xAngle = 0.0f;
	m_xAngle = 0.0f;
	m_xScale = 1.0f;
	m_yScale = 1.0f;
	m_zScale = 1.0f;

	m_directional_light_origin = XMVectorSet(-1.0f, 0.0f, -1.0f, 0.0f);
	m_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	m_ambient_light_colour = XMVectorSet(0.25f, 0.25f, 0.25f, 1.0f);

	for (int i = 0; i < m_numberOfParticles; i++) {
		m_free.push_back(new Particle());
	}
	m_timePrevious = float(timeGetTime()) / 1000.0f;
	m_untilParticle = 10.0f;
}

ParticleGenerator::~ParticleGenerator()
{
	delete m_pObject;
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();

	std::list<Particle*>::iterator it;
	for (it = m_free.end(); it != m_free.begin(); it--)
	{
		m_free.remove(*it);
	}
	for (it = m_active.end(); it != m_active.begin(); it--)
	{
		m_active.remove(*it);
	}
}

HRESULT ParticleGenerator::CompileShaders()
{
	HRESULT hr = S_OK;

	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("particle_shader.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("particle_shader.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	//Create shader objects
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr))
	{
		return hr;
	}

	//Set the shader objects as active
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	// Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 112; // must be a multiple of 16
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	return S_OK;
}

HRESULT ParticleGenerator::AddTexture(char * filename)
{
	return D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, filename, NULL, NULL, &m_pTexture0, NULL);
}

void ParticleGenerator::Draw(XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* cameraPosition)
{
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	XMMATRIX world;
	m_untilParticle -= Time::getInstance().deltaTime / 100;
	std::list<Particle*>::iterator it;

	if (m_untilParticle <= 0.0f)
	{
		if (m_isActive)//a bool to check if the particle engine is on or off. Make a getter/setter and use it in main
		{
			it = m_free.begin();//point to the beggining of the free list
								//add a new particle to the back of m_active from the front of m_free
			if (m_free.size() != NULL)//safety check
			{
				switch (type)//the name of my enum
				{
				case RAINBOW_FOUNTAIN:
				{
					m_age = 2.0f;
					m_untilParticle = 0.008f;
					////////////////////////initialise the particle NOTE: all of this is adjustable for different effects////////////////////////
					(*it)->colour = XMFLOAT4(RandomZeroToOne(), RandomZeroToOne(), RandomZeroToOne(), RandomZeroToOne());
					(*it)->gravity = 4.5f;
					(*it)->position = XMFLOAT3(0.0f, 1.0f, 3.0f);
					(*it)->velocity = XMFLOAT3(RandomNegOneToPosOne(), 2.50f, RandomNegOneToPosOne());
					////////////////////////////////////////////////////////////////////////////////////////////////
					break;
				}
				default:
				{
					break;
				}
				}
				(*it)->age = 0.0f;//set age to 0. this is used for knowing when to delete the particle

								  //////add the particle from the front of the available list to the back of the active list and remove it
				m_active.push_back(*it);
				m_free.pop_front();
			}
		}
		else m_untilParticle = 0.001f;
	}

	if (m_active.size() != NULL)//safety check
	{
		it = m_active.begin();//point the iterator to the front of the active list ready for processing
		while (it != m_active.end())//move all of the particles
		{

			switch (type)
			{
			case RAINBOW_FOUNTAIN:
			{
				/////////////////////////ALL of this is adjustable for different effects///////////////////////////////////////////////////////////
				(*it)->age += Time::getInstance().deltaTime / 1000;
				(*it)->velocity.y -= (*it)->gravity*(Time::getInstance().deltaTime / 100);
				(*it)->position.x += (*it)->velocity.x*(Time::getInstance().deltaTime / 100);
				(*it)->position.y += (*it)->velocity.y*(Time::getInstance().deltaTime / 100);
				(*it)->position.z += (*it)->velocity.z*(Time::getInstance().deltaTime / 100);
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				break;
			}
			default:
			{
				break;
			}
			}
			world = XMMatrixIdentity();
			switch (type)
			{
			case RAINBOW_FOUNTAIN:
			{
				/*set scale and world transforms here*/
				world *= XMMatrixScaling(0.3f, 0.3f, 0.3f);
				//world *= XMMatrixRotationY(XMConvertToRadians(XM_PI));
				LookAt_XZ(cameraPosition->x, cameraPosition->z);
				world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
				break;
			}
			default:
			{
				break;
			}
			}
			world *= XMMatrixTranslation((*it)->position.x, (*it)->position.y, (*it)->position.z);

			//constant buffer stuff for shader
			PARTICLE_CONSTANT_BUFFER pcb;
			pcb.WorldViewProjection = (world) * (*view)*(*projection);
			pcb.color = (*it)->colour;

			m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
			m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &pcb, 0, 0);

			//set the shader objects as active
			m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
			m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
			m_pImmediateContext->IASetInputLayout(m_pInputLayout);

			m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

			m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_pImmediateContext->RSSetState(m_pRasterParticle);//set backface culling to on
			m_pImmediateContext->Draw(6, 0);//draw the particle
			m_pImmediateContext->RSSetState(m_pRasterSolid);//set backface culling to off
			if ((*it)->age >= m_age)//check the age of the current particle
			{
				it++;
				m_active.front()->age = m_age;
				m_active.front()->position = { (RandomNegOneToPosOne() + m_x * 10)*(RandomZeroToOne() * 10),m_y + 5.0f, m_z + 7.0f };
				m_active.front()->velocity = { 0.0f, 4.50f, RandomNegOneToPosOne() };

				if (!m_playOnce)
					m_free.push_back(m_active.front());//move the (now previously) current active particle to the back of the pool			

				m_active.pop_front();//remove the particle			
			}
			else it++;
		}//end of while
	}//end of if(m_active.size()!=NULL)
}

void ParticleGenerator::DrawOne(Particle* particle, XMMATRIX * view, XMMATRIX * projection, XMFLOAT3 * cameraPosition)
{
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	m_pImmediateContext->RSSetState(m_pRasterParticle);

	float dx = cameraPosition->x - particle->position.x;
	float dz = cameraPosition->z - particle->position.z;
	float m_yAngle = atan2(dx, dz) * (180.0 / XM_PI);

	XMMATRIX world;
	PARTICLE_CONSTANT_BUFFER cb_values;
	world = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixTranslation(particle->position.x, particle->position.y, particle->position.z);
	cb_values.WorldViewProjection = world * (*view) * (*projection);
	cb_values.color = particle->colour;

	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &cb_values, 0, 0);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	// Select which primitive type to use
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw the vertex buffer to the back buffer
	m_pImmediateContext->Draw(6, 0);
}

void ParticleGenerator::LookAt_XZ(float x, float z)
{
	float dx, dz;

	dx = x - m_x;
	dz = z - m_z;

	m_yAngle = atan2(dx, dz) * (180.0 / XM_PI);
}

void ParticleGenerator::MoveForward(float distance)
{
	m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance;
	m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance;
}

bool ParticleGenerator::CheckCollision(ParticleGenerator * other)
{
	if (other == this)
		return false;

	float x = abs(XMVectorGetX(other->GetBoundingSphereWorldSpacePosition() - GetBoundingSphereWorldSpacePosition()));
	float y = abs(XMVectorGetY(other->GetBoundingSphereWorldSpacePosition() - GetBoundingSphereWorldSpacePosition()));
	float z = abs(XMVectorGetZ(other->GetBoundingSphereWorldSpacePosition() - GetBoundingSphereWorldSpacePosition()));

	float distance = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

	if (distance <= other->GetBoundingSphereRadius() + GetBoundingSphereRadius())
		return true;
	else
		return false;
}

bool ParticleGenerator::CheckCollision(ParticleGenerator * other, float xOffset, float yOffset, float zOffset)
{
	if (other == this)
		return false;

	float x = abs(XMVectorGetX(other->GetBoundingSphereWorldSpacePosition()) - (XMVectorGetX(GetBoundingSphereWorldSpacePosition()) + xOffset));
	float y = abs(XMVectorGetY(other->GetBoundingSphereWorldSpacePosition()) - (XMVectorGetY(GetBoundingSphereWorldSpacePosition()) + yOffset));
	float z = abs(XMVectorGetZ(other->GetBoundingSphereWorldSpacePosition()) - (XMVectorGetZ(GetBoundingSphereWorldSpacePosition()) + zOffset));

	float distance = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

	if (distance <= other->GetBoundingSphereRadius() + GetBoundingSphereRadius())
		return true;
	else
		return false;
}

void ParticleGenerator::SetXPos(float x)
{
	m_x = x;
}

void ParticleGenerator::SetYPos(float y)
{
	m_y = y;
}

void ParticleGenerator::SetZPos(float z)
{
	m_z = z;
}

void ParticleGenerator::SetPos(float x, float y, float z)
{
	SetXPos(x);
	SetYPos(y);
	SetZPos(z);
}

void ParticleGenerator::SetXRot(float x)
{
	m_xAngle = x;
}

void ParticleGenerator::SetYRot(float y)
{
	m_yAngle = y;
}

void ParticleGenerator::SetZRot(float z)
{
	m_zAngle = z;
}

void ParticleGenerator::SetRot(float x, float y, float z)
{
	SetXRot(x);
	SetYRot(y);
	SetZRot(z);
}

void ParticleGenerator::SetXScale(float x)
{
	m_xScale = x;
}

void ParticleGenerator::SetYScale(float y)
{
	m_yScale = y;
}

void ParticleGenerator::SetZScale(float z)
{
	m_zScale = z;
}

void ParticleGenerator::SetScale(float x, float y, float z)
{
	SetXScale(x);
	SetYScale(y);
	SetZScale(z);
}

void ParticleGenerator::AddXPos(float x)
{
	m_x += x;
}

void ParticleGenerator::AddYPos(float y)
{
	m_y += y;
}

void ParticleGenerator::AddZPos(float z)
{
	m_z += z;
}

void ParticleGenerator::AddPos(float x, float y, float z)
{
	AddXPos(x);
	AddYPos(y);
	AddZPos(z);
}

void ParticleGenerator::AddXRot(float x)
{
	m_xAngle += x;
}

void ParticleGenerator::AddYRot(float y)
{
	m_yAngle += y;
}

void ParticleGenerator::AddZRot(float z)
{
	m_zAngle += z;
}

void ParticleGenerator::AddRot(float x, float y, float z)
{
	AddXRot(x);
	AddYRot(y);
	AddZRot(z);
}

void ParticleGenerator::AddXScale(float x)
{
	m_xScale += x;
}

void ParticleGenerator::AddYScale(float y)
{
	m_yScale += y;
}

void ParticleGenerator::AddZScale(float z)
{
	m_zScale += z;
}

void ParticleGenerator::AddScale(float x, float y, float z)
{
	AddXScale(x);
	AddYScale(y);
	AddZScale(z);
}

float ParticleGenerator::GetXPos()
{
	return m_x;
}

float ParticleGenerator::GetYPos()
{
	return m_y;
}

float ParticleGenerator::GetZPos()
{
	return m_z;
}

float ParticleGenerator::GetXRot()
{
	return m_xAngle;
}

float ParticleGenerator::GetYRot()
{
	return m_yAngle;
}

float ParticleGenerator::GetZRot()
{
	return m_zAngle;
}

float ParticleGenerator::GetXScale()
{
	return m_xScale;
}

float ParticleGenerator::GetYScale()
{
	return m_yScale;
}

float ParticleGenerator::GetZScale()
{
	return m_zScale;
}

void ParticleGenerator::SetDirectionalLightOrigin(XMVECTOR origin)
{
	m_directional_light_origin = origin;
}

void ParticleGenerator::SetDirectionalLightColour(XMVECTOR colour)
{
	m_directional_light_colour = colour;
}

void ParticleGenerator::SetAmbientLightColour(XMVECTOR colour)
{
	m_ambient_light_colour = colour;
}

XMVECTOR ParticleGenerator::GetDirectionalLightOrigin()
{
	return m_directional_light_origin;
}

XMVECTOR ParticleGenerator::GetDirectionalLightColour()
{
	return m_directional_light_colour;
}

XMVECTOR ParticleGenerator::GetAmbientLightColour()
{
	return m_ambient_light_colour;
}

XMVECTOR ParticleGenerator::GetBoundingSphereWorldSpacePosition()
{
	XMMATRIX world;
	world = XMMatrixScaling(m_xScale, m_yScale, m_zScale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	XMVECTOR offset;
	offset = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0.0);
	offset = XMVector3Transform(offset, world);

	return offset;
}

float ParticleGenerator::GetBoundingSphereRadius()
{
	return m_bounding_sphere_radius * ((m_xScale + m_yScale + m_zScale) / 3.0);
}

int ParticleGenerator::CreateParticle()
{
	HRESULT hr = S_OK;
	XMFLOAT3 vertices[6] =//verts for the quad NOTE: could be changed to make different shapes??
	{
		XMFLOAT3(-1.0f,-1.0f,0.0f),
		XMFLOAT3(1.0f,1.0f,0.0f),
		XMFLOAT3(-1.0f,1.0f,0.0f),
		XMFLOAT3(-1.0f,-1.0f,0.0f),
		XMFLOAT3(1.0f,-1.0f,0.0f),
		XMFLOAT3(1.0f,1.0f,0.0f),
	};
	D3D11_RASTERIZER_DESC rasterizer_desc;
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;//sets no culling
	hr = m_pD3DDevice->CreateRasterizerState(&rasterizer_desc, &m_pRasterSolid);
	rasterizer_desc.CullMode = D3D11_CULL_BACK;//sets backface culling
	hr = m_pD3DDevice->CreateRasterizerState(&rasterizer_desc, &m_pRasterParticle);
	//create the vert buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;//both cpu and gpu
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * 6/*VERTCOUNT*/;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//edit on the cpu

	hr = m_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);

	if (FAILED(hr)) return 0;

	//copy verts to buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//mapping = locking the buffer which allows writing
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, vertices, sizeof(vertices));//copy data
	//unlock the buffer
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);

	/*CalcModelCentrePoint();
	CalcBoundingSphereRadius();*/
	CompileShaders();

	return 0;
}
