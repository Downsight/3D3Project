#include "Camera.h"


Camera::Camera()
{
	m_cameraPos = XMVectorSet(0, 0, 0, 0);
	m_angleX = XM_PIDIV2;
	m_angleZ = 0;
	SetLookAtVec();
	dirtyCanera = true;
}

Camera::~Camera()
{
	
}

XMVECTOR Camera::GetCameraPos()
{
	return m_cameraPos;
}

void Camera::SetCameraPos(XMVECTOR pos)
{
	m_cameraPos = m_cameraPos + pos;
	dirtyCanera = true;
}

XMMATRIX Camera::ViewMatrix()
{
	return XMMatrixLookAtLH(m_cameraPos, GetLookAtVec(), XMVectorSet(0, 1, 0, 0));
	/*return XMMatrixLookAtLH(XMVectorSet(0, 60, 0, 0), XMVectorSet(0.4f, 59.8f, 0.4f, 0.0f), XMVectorSet(0, 1, 0, 0));*/
}

XMMATRIX Camera::ProjectionMatrix()
{
	return XMMatrixPerspectiveFovLH(XM_PI *0.45f, 640 / 480, 0.5f, 150);
}

XMFLOAT4X4* Camera::GetPFLOAT()
{
	XMFLOAT4X4* P = new XMFLOAT4X4[1];
	XMStoreFloat4x4(P, ProjectionMatrix());
	return P;
}

XMFLOAT4X4* Camera::GetVFLOAT()
{
	XMFLOAT4X4* VI = new XMFLOAT4X4[1];
	XMStoreFloat4x4(VI, XMMatrixInverse(nullptr, ViewMatrix()));
	return VI;
}

XMVECTOR Camera::GetLookAtVec()
{
	return m_lookAtVec;
}

XMFLOAT3 Camera::GetCameraPosF()
{
	return XMFLOAT3(XMVectorGetByIndex(m_cameraPos, 0), XMVectorGetByIndex(m_cameraPos, 1), XMVectorGetByIndex(m_cameraPos, 2));
}

void Camera::SetLookAtVec()
{
	float X, Y, Y2, Z;
	X = cos(GetAngleXZ());
	Y = sin(GetAngleYZ());
	Y2 = sin(GetAngleYZ() + XM_PIDIV2);
	Z = sin(GetAngleXZ());
	XMVECTOR Look = XMVectorSet(X, Y, Z, 0);
	XMVECTOR LookSide = XMVectorSet(X, Y2, Z, 0);
	m_lookAtVec = m_cameraPos + XMVector3Normalize(Look);
	m_sideVec = m_cameraPos + XMVector3Normalize(LookSide);
	dirtyCanera = true;
}

bool Camera::MovingCamera(float time)
{
	bool Change = false;
	XMVECTOR move = XMVectorSet(0, 0, 0, 0);
	if (GetAsyncKeyState(VK_ADD))
	{
		m_speed += time;
	}
	if (GetAsyncKeyState(VK_SUBTRACT))
	{
		m_speed -= time;
	}
	if (GetAsyncKeyState('Q'))
	{
		SetAngleXZ(1.0f * time);
		Change = true;
	}
	if (GetAsyncKeyState('E'))
	{
		SetAngleXZ(-1.0f * time);
		Change = true;
	}
	if (GetAsyncKeyState('R'))
	{
		SetAngleYZ(1.0f * time);
		Change = true;
	}
	if (GetAsyncKeyState('F'))
	{
		SetAngleYZ(-1.0f * time);
		Change = true;
	}
	if (GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP))
	{
		move += XMVectorSet(cos(GetAngleXZ()) * time * m_speed, 0, sin(GetAngleXZ()) * time * m_speed, 0);
		Change = true;
	}
	if (GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN))
	{
		move -= XMVectorSet(cos(GetAngleXZ()) * time * m_speed, 0, sin(GetAngleXZ()) * time * m_speed, 0);
		Change = true;
	}
	if (GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT))
	{
		move += XMVectorSet(cos(GetAngleXZ() + XM_PIDIV2) * time * m_speed, 0, sin(GetAngleXZ() + XM_PIDIV2) * time * m_speed, 0);
		Change = true;
	}
	if (GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT))
	{
		move += XMVectorSet(cos(GetAngleXZ() - XM_PIDIV2) * time * m_speed, 0, sin(GetAngleXZ() - XM_PIDIV2) * time * m_speed, 0);
		Change = true;
	}
	if (GetAsyncKeyState(VK_LSHIFT))
	{
		move += XMVectorSet(0, 1 * time * m_speed, 0, 0);
		Change = true;
	}
	if (GetAsyncKeyState(VK_LCONTROL))
	{
		move += XMVectorSet(0, -1 * time * m_speed, 0, 0);
		Change = true;
	}
	if (Change == true)
	{
		SetCameraPos(move);
		SetLookAtVec();
	}
	return false;
}

float Camera::GetAngleYZ()
{
	return m_angleZ;
}

void Camera::SetAngleYZ(float degreZ)
{
	if ((m_angleZ + degreZ) >= XM_PIDIV2)
	{
		m_angleZ = XM_PIDIV2;
	}
	else if ((m_angleZ + degreZ) <= -XM_PIDIV2)
	{
		m_angleZ = -XM_PIDIV2;
	}
	else
	{
		m_angleZ += degreZ;
	}
}

float Camera::GetAngleXZ()
{
	return m_angleX;
}

void Camera::SetAngleXZ(float degreX)
{
	if ((m_angleX + degreX) >= 360)
	{
		m_angleX = 0;
	}
	else if ((m_angleX + degreX) <= -360)
	{
		m_angleX = 0;
	}
	else
	{
		m_angleX += degreX;
	}
}

void Camera::SetCamera(XMFLOAT4 pos, float anglex, float anglez)
{
	m_cameraPos = XMVectorSet(pos.x, pos.y, pos.z, 0);
	m_angleX = anglex;
	m_angleZ = anglez;
	SetLookAtVec();
	dirtyCanera = true;
}

void * Camera::GetCameraData()
{
	return (void*)&m_constCamera;
}

bool Camera::Frame()
{
	if (dirtyCanera)
	{
		m_constCamera.CameraPos = XMFLOAT4(XMVectorGetByIndex(m_cameraPos, 0), XMVectorGetByIndex(m_cameraPos, 1), XMVectorGetByIndex(m_cameraPos, 2), 0);
		XMStoreFloat4x4(&m_constCamera.ViewProjection, XMMatrixTranspose(ViewMatrix() * ProjectionMatrix()));
		dirtyCanera = false;
		return true;
	}
	return false;
}