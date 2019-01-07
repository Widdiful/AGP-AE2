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

class Skybox
{
private:
	ID3D11Device*				m_pD3DDevice;
	ID3D11Buffer*				m_pVertexBuffer;
	ID3D11Buffer*				m_pConstantBuffer0;
	ID3D11DeviceContext*		m_pImmediateContext;
	ID3D11VertexShader*			m_pVertexShader;
	ID3D11PixelShader*			m_pPixelShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11ShaderResourceView*	m_pSkybox;
	ID3D11SamplerState*			m_pSampler0;

public:
	Skybox(ID3D11Device* device, ID3D11Buffer* vertexBuffer, ID3D11Buffer* constantBuffer, ID3D11DeviceContext* immediateContext, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11InputLayout* inputLayout, ID3D11ShaderResourceView* skyboxShader, ID3D11SamplerState* sampler);
	~Skybox();

	HRESULT CreateSkybox();
	void SetTexture(LPCSTR texture);
	void DrawSkybox(Camera* camera, XMMATRIX* view, XMMATRIX* projection, ID3D11RasterizerState* rasterSkybox, ID3D11DepthStencilState* depthWriteSkybox, ID3D11RasterizerState* rasterSolid, ID3D11DepthStencilState* depthWriteSolid);
};

