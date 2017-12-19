#pragma once
#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <vector>
#include <Renderer\IRenderer.h>
#include "Camera.h"

class CameraManager
{
public:
	CameraManager();
	CameraManager(D3DPROJECT::RENDERER::IRenderer* rend);
	~CameraManager();

	int NewCamera();
	Camera* GetCamera(int nrCamera);
	void Frame();

private:
	std::vector<Camera*> m_Cameras;
	D3DPROJECT::RENDERER::IRenderer* renderer = nullptr;
};



#endif