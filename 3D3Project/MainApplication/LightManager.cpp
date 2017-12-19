#include "LightManager.h"

LightManager::LightManager()
{

}

LightManager::~LightManager()
{

}

void LightManager::AddLight(XMFLOAT4 position, XMFLOAT4 color, XMFLOAT4 Range)
{
	Light* temp = new Light(position, color, Range);
	m_lights.push_back(temp);
}

void LightManager::InitializBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC _lightBufferDesc;
	ZeroMemory(&_lightBufferDesc, sizeof(D3D11_BUFFER_DESC));

	_lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	_lightBufferDesc.ByteWidth = sizeof(XMFLOAT4) * 600;
	_lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	_lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	_lightBufferDesc.MiscFlags = 0;

	//skapar constant buffer
	HRESULT hr = device->CreateBuffer(&_lightBufferDesc, NULL, &m_lightBuffer);

}

void LightManager::SetConstbuffer(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE _mappedResource;

	HRESULT hr = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &_mappedResource);

	for (int i = 0; i < m_lights.size(); i++)
	{
		m_lightningArray.LightPos[i] = m_lights.at(i)->GetPos();
		m_lightningArray.LightColor[i] = m_lights.at(i)->GetColor();
		m_lightningArray.LightRange[i] = XMFLOAT4(m_lights.at(i)->GetRange().x, m_lights.size(), m_lights.at(i)->GetRange().z, m_lights.at(i)->GetRange().w);
	}

	memcpy(_mappedResource.pData, &m_lightningArray, sizeof(LightData));

	deviceContext->Unmap(m_lightBuffer, 0);

	deviceContext->PSSetConstantBuffers(1, 1, &m_lightBuffer);
}