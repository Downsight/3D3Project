#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <vector>
#include <iostream>

using namespace std;
using namespace DirectX;

class Light
{
private:
	
public:
	Light();
	Light(XMFLOAT4 position, XMFLOAT4 color, XMFLOAT4 Range);
	~Light();
	XMFLOAT4 GetPos();
	XMFLOAT4 GetColor();
	XMFLOAT4 GetRange();
	void SetPos(XMFLOAT4 position);
	void SetColor(XMFLOAT4 color);
	void SetRange(XMFLOAT4 Range);
	


private:
	XMFLOAT4 m_lightPos;
	XMFLOAT4 m_lightColor;
	XMFLOAT4 m_lightRange;
};

#endif
