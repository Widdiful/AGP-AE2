#pragma once
#include "objfilemodel.h"
#include <map>


class Model
{
protected:
	ID3D11Device*			m_pD3DDevice;
	ID3D11DeviceContext*	m_pImmediateContext;

	ObjFileModel*			m_pObject;
	ID3D11VertexShader*		m_pVShader;
	ID3D11PixelShader*		m_pPShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pConstantBuffer;
	ID3D11ShaderResourceView* m_pTexture0;
	ID3D11SamplerState*		m_pSampler0;

	XMVECTOR				m_directional_light_origin;
	XMVECTOR				m_directional_light_colour;
	XMVECTOR				m_ambient_light_colour;

	float					m_x, m_y, m_z;
	float					m_xAngle, m_yAngle, m_zAngle;
	float					m_xScale, m_yScale, m_zScale;
	float					m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, m_bounding_sphere_radius;
	float					m_cube_boundsX, m_cube_boundsY, m_cube_boundsZ;

	XMFLOAT3 CalculateModelCentrePoint();
	float CalculateBoundingSphereRadius();
	XMFLOAT3 CalculateCubeBounds();

	static map<char*, ObjFileModel*> m_modelMap;
public:
	Model(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext);
	~Model();

	HRESULT LoadObjModel(char* filename);
	HRESULT AddTexture(char* filename);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetSampler(ID3D11SamplerState* sampler);
	void Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	void LookAt_XZ(float x, float z);
	void MoveForward(float distance);
	bool CheckCollision(Model* other);
	bool CheckCollision(Model* other, float xOffset, float yOffset, float zOffset);
	virtual void Update();
	ObjFileModel* GetObject();

	// POSITION INFO

	void SetXPos(float x);
	void SetYPos(float y);
	void SetZPos(float z);
	void SetPos(float x, float y, float z);

	void AddXPos(float x);
	void AddYPos(float y);
	void AddZPos(float z);
	void AddPos(float x, float y, float z);

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

	// LIGHTING INFO

	void SetDirectionalLightOrigin(XMVECTOR origin);
	void SetDirectionalLightColour(XMVECTOR colour);
	void SetAmbientLightColour(XMVECTOR colour);

	XMVECTOR GetDirectionalLightOrigin();
	XMVECTOR GetDirectionalLightColour();
	XMVECTOR GetAmbientLightColour();

	// COLLISION INFO

	XMVECTOR GetBoundingSphereWorldSpacePosition();
	float GetBoundingSphereRadius();
	XMFLOAT3 GetCubeBounds();
};

