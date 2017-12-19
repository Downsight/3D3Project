#include "Light.h"

Light::Light()
{

}

Light::Light(XMFLOAT4 position, XMFLOAT4 color, XMFLOAT4 Range)
{
	m_lightPos = position;
	m_lightColor = color;
	m_lightRange = Range;
}

Light::~Light()
{

}

XMFLOAT4 Light::GetPos()
{
	return m_lightPos;
}

XMFLOAT4 Light::GetColor()
{
	return m_lightColor;
}

XMFLOAT4 Light::GetRange()
{
	return m_lightRange;
}

void Light::SetPos(XMFLOAT4 position)
{
	m_lightPos = position;
}

void Light::SetColor(XMFLOAT4 color)
{
	m_lightColor = color;
}

void Light::SetRange(XMFLOAT4 Range)
{
	m_lightRange = Range;
}