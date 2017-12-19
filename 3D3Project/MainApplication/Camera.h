#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <vector>
#include <d3dcompiler.h>

using namespace std;
using namespace DirectX;

#pragma once

class Camera
{
public:
	Camera();
	~Camera();
	XMVECTOR GetCameraPos();
	XMVECTOR GetLookAtVec();
	void SetLookAtVec();
	float GetAngleYZ();
	void SetAngleYZ(float);
	float GetAngleXZ();
	void SetAngleXZ(float);
	XMFLOAT3 GetCameraPosF();
	XMFLOAT4X4* GetPFLOAT();
	XMFLOAT4X4* GetVFLOAT();
	XMMATRIX ViewMatrix();
	XMMATRIX ProjectionMatrix();
	bool MovingCamera(float time);
	void SetCameraPos(XMVECTOR pos);

	void SetCamera(XMFLOAT4 pos, float anglex, float anglez);
	
	void* GetCameraData();
	int GetCameraIndex() { return cameraIndex; }
	void SetCameraIndex(int index) { cameraIndex = index; }

	bool Frame();

private:
	void UpdateDirty();

	struct CameraType
	{
		XMFLOAT4 CameraPos;
		XMFLOAT4X4 ViewProjection;
	};
	XMVECTOR m_cameraPos;
	XMVECTOR m_lookAtVec, m_sideVec;
	float m_lookAtXZ;
	float m_lookAtYZ;
	float m_angleX, m_angleZ;
	CameraType m_constCamera;
	int cameraIndex = -1;
	float m_speed = 12.0;
	bool dirtyCanera = false;
};

#endif