#pragma once
#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "Light.h"
#include <vector>

class LightManager
{
private:
	struct LightData
	{
		XMFLOAT4 LightPos[200];
		XMFLOAT4 LightColor[200];
		XMFLOAT4 LightRange[200];
	};

public:
	LightManager();
	~LightManager();
	void AddLight(XMFLOAT4 position, XMFLOAT4 color, XMFLOAT4 Range);
	void InitializBuffer(ID3D11Device* device);
	void SetConstbuffer(ID3D11DeviceContext* deviceContext);

private:
	std::vector<Light*> m_lights;
	ID3D11Buffer* m_lightBuffer = nullptr;
	LightData m_lightningArray = {};
};



#endif