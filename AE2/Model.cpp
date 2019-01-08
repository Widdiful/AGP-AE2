#include "Model.h"
#include <math.h>

struct MODEL_CONSTANT_BUFFER {
	XMMATRIX WorldViewProjection;
	XMVECTOR directional_light_vector;
	XMVECTOR directional_light_colour;
	XMVECTOR ambient_light_colour;
};

XMFLOAT3 Model::CalculateModelCentrePoint()
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

float Model::CalculateBoundingSphereRadius()
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

Model::Model(ID3D11Device * pD3DDevice, ID3D11DeviceContext * pImmediateContext)
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

	m_pTexture0 = NULL;
	m_pSampler0 = NULL;
}

Model::~Model()
{
	delete m_pObject;
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
}

HRESULT Model::LoadObjModel(char * filename)
{
	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);

	if (m_pObject->filename == "FILE NOT LOADED") return S_FALSE;

	HRESULT hr = S_OK;

	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("model_shader.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("model_shader.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

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
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

	CalculateModelCentrePoint();
	CalculateBoundingSphereRadius();

	return S_OK;
}

HRESULT Model::AddTexture(char * filename)
{
	return D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, filename, NULL, NULL, &m_pTexture0, NULL);
}

void Model::SetTexture(ID3D11ShaderResourceView * texture)
{
	m_pTexture0 = texture;
}

void Model::SetSampler(ID3D11SamplerState * sampler)
{
	m_pSampler0 = sampler;
}

void Model::Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
	// Scale, rotate and position model correctly
	//XMMATRIX world;
	MODEL_CONSTANT_BUFFER model_cb_values;
	/*world = XMMatrixScaling(m_xScale, m_yScale, m_zScale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);*/
	model_cb_values.WorldViewProjection = (*world) * (*view) * (*projection);

	XMMATRIX transpose;
	transpose = XMMatrixTranspose(*world);
	model_cb_values.directional_light_colour = m_directional_light_colour;
	model_cb_values.ambient_light_colour = m_ambient_light_colour;
	model_cb_values.directional_light_vector = XMVector3Transform(m_directional_light_origin, transpose);
	model_cb_values.directional_light_vector = XMVector3Normalize(model_cb_values.directional_light_vector);

	// Add to buffer
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// Set shaders
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	// Apply textures
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);

	// Draw the object
	m_pObject->Draw();
}

void Model::LookAt_XZ(float x, float z)
{
	float dx, dz;

	dx = x - m_x;
	dz = z - m_z;

	m_yAngle = atan2(dx, dz) * (180.0 / XM_PI);
}

void Model::MoveForward(float distance)
{
	m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance;
	m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance;
}

bool Model::CheckCollision(Model * other)
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

bool Model::CheckCollision(Model * other, float xOffset, float yOffset, float zOffset)
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

void Model::Update()
{

}

ObjFileModel * Model::GetObject()
{
	return m_pObject;
}

void Model::SetXPos(float x)
{
	m_x = x;
}

void Model::SetYPos(float y)
{
	m_y = y;
}

void Model::SetZPos(float z)
{
	m_z = z;
}

void Model::SetPos(float x, float y, float z)
{
	SetXPos(x);
	SetYPos(y);
	SetZPos(z);
}

void Model::SetXRot(float x)
{
	m_xAngle = x;
}

void Model::SetYRot(float y)
{
	m_yAngle = y;
}

void Model::SetZRot(float z)
{
	m_zAngle = z;
}

void Model::SetRot(float x, float y, float z)
{
	SetXRot(x);
	SetYRot(y);
	SetZRot(z);
}

void Model::SetXScale(float x)
{
	m_xScale = x;
}

void Model::SetYScale(float y)
{
	m_yScale = y;
}

void Model::SetZScale(float z)
{
	m_zScale = z;
}

void Model::SetScale(float x, float y, float z)
{
	SetXScale(x);
	SetYScale(y);
	SetZScale(z);
}

void Model::AddXPos(float x)
{
	m_x += x;
}

void Model::AddYPos(float y)
{
	m_y += y;
}

void Model::AddZPos(float z)
{
	m_z += z;
}

void Model::AddPos(float x, float y, float z)
{
	AddXPos(x);
	AddYPos(y);
	AddZPos(z);
}

void Model::AddXRot(float x)
{
	m_xAngle += x;
}

void Model::AddYRot(float y)
{
	m_yAngle += y;
}

void Model::AddZRot(float z)
{
	m_zAngle += z;
}

void Model::AddRot(float x, float y, float z)
{
	AddXRot(x);
	AddYRot(y);
	AddZRot(z);
}

void Model::AddXScale(float x)
{
	m_xScale += x;
}

void Model::AddYScale(float y)
{
	m_yScale += y;
}

void Model::AddZScale(float z)
{
	m_zScale += z;
}

void Model::AddScale(float x, float y, float z)
{
	AddXScale(x);
	AddYScale(y);
	AddZScale(z);
}

float Model::GetXPos()
{
	return m_x;
}

float Model::GetYPos()
{
	return m_y;
}

float Model::GetZPos()
{
	return m_z;
}

float Model::GetXRot()
{
	return m_xAngle;
}

float Model::GetYRot()
{
	return m_yAngle;
}

float Model::GetZRot()
{
	return m_zAngle;
}

float Model::GetXScale()
{
	return m_xScale;
}

float Model::GetYScale()
{
	return m_yScale;
}

float Model::GetZScale()
{
	return m_zScale;
}

void Model::SetDirectionalLightOrigin(XMVECTOR origin)
{
	m_directional_light_origin = origin;
}

void Model::SetDirectionalLightColour(XMVECTOR colour)
{
	m_directional_light_colour = colour;
}

void Model::SetAmbientLightColour(XMVECTOR colour)
{
	m_ambient_light_colour = colour;
}

XMVECTOR Model::GetDirectionalLightOrigin()
{
	return m_directional_light_origin;
}

XMVECTOR Model::GetDirectionalLightColour()
{
	return m_directional_light_colour;
}

XMVECTOR Model::GetAmbientLightColour()
{
	return m_ambient_light_colour;
}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition()
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

float Model::GetBoundingSphereRadius()
{
	return m_bounding_sphere_radius * ((m_xScale + m_yScale + m_zScale) / 3.0);
}
