#include "CameraManager.h"

CameraManager::CameraManager()
{

}

CameraManager::CameraManager(D3DPROJECT::RENDERER::IRenderer * rend)
{
	renderer = rend;
}

CameraManager::~CameraManager()
{
	for (int i = 0; m_Cameras.size() > i; i++)
	{
		delete m_Cameras[i];
	}
	m_Cameras.clear();
}

int CameraManager::NewCamera()
{
	Camera* tempCamera = new Camera;
	tempCamera->SetCameraIndex(renderer->CreateCameraBuffer());
	if (tempCamera->GetCameraIndex() < 0)
	{
		return -1;
	}

	m_Cameras.push_back(tempCamera);
	 
	return m_Cameras.size() - 1;
}

Camera* CameraManager::GetCamera(int nrCamera)
{
	if (nrCamera < m_Cameras.size())
	{
		return m_Cameras.at(nrCamera);
	}
}

void CameraManager::Frame()
{
	for (auto& camera : m_Cameras)
	{
		if (camera->Frame())
		{
			renderer->SetCameraBuffer(camera->GetCameraData(), camera->GetCameraIndex());
		}
	}
}
