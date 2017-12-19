#pragma once
#ifndef TESTPROGRAM_H
#define TESTPROGRAM_H

#include "ModelManager.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "EntityManager.h"
#include "TimerClass.h"
#include <Window\IWindow.h>
#include <Renderer\IRenderer.h>

#define type 1




class TestProgram
{
public:
	TestProgram();
	~TestProgram();
	WPARAM RunTestProgram();
private:

	CameraManager* m_CameraManager = nullptr;
	ModelManager* m_ModelManager = nullptr;
	EntityManager* m_EntityManager = nullptr;
	D3DPROJECT::WINDOW::IWindow* window = nullptr;
	D3DPROJECT::RENDERER::IRenderer* renderer = nullptr;
	TimerClass* m_timer = nullptr;
	LightManager* m_LightManager = nullptr;

	MSG m_winMSG;
};



#endif