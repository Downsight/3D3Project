#include "TestProgram.h"
#include <Windows.h>
#include "cpuWallTime.h"
#include <fstream>
#include <iostream>
TestProgram::TestProgram()
{

}

TestProgram::~TestProgram()
{
	
}

WPARAM TestProgram::RunTestProgram()
{
	cpuWalltime Time;
	cpuWalltime InitTime;
	cpuWalltime renderTime;
	cpuWalltime cleanTime;
	cpuWalltime presentTime;
	cpuWalltime modelTime;
	cpuWalltime updateTime;
	cpuWalltime loopTime;

	int api;
	std::cout << "Which api should be used:" << std::endl << "1 for D11" << std::endl << "2 for D12" << std::endl;
	std::cin >> api;


	for (int testRun = 0; testRun < 10; testRun++)
	{
		int error = 0;
		D3DPROJECT::WINDOW::InitializeInfo info;
		window = D3DPROJECT::WINDOW::CreateNewWindow();
		window->Initialize(info);
		
		InitTime.timeStart();
		renderer = D3DPROJECT::RENDERER::CreateNewRenderer(api);
		error = renderer->Initialize((HWND)(window->GetWindowHandle()), window->GetWidth(), window->GetHeight());
		InitTime.timeEnd();
		

		if (error < 0)
		{
			std::cout << "error in Init Renderer" << std::endl;
			goto END;
		}

		m_CameraManager = new CameraManager(renderer);
		m_ModelManager = new ModelManager(renderer);
		m_EntityManager = new EntityManager(renderer);

		if (window)
		{
			m_timer = new TimerClass();

			error = m_EntityManager->NewEntityType(4, 0, 0, 0.1, 0.1, 0.1);

			if (error < 0)
			{
				std::cout << "error in New entityType" << std::endl;
				goto END;
			}
			for (int i = 0; i < 200; i++)
			{
				m_EntityManager->NewEntity(rand() % 100, rand() % 100, rand() % 100, 0.1, 0.1, 0.1, 0);
			}
			
			modelTime.timeStart();
			error = m_ModelManager->NewModel("Resource/mobileviking.obj", "Resource/mobileviking.mtl");
			modelTime.timeEnd();
			
			if (error < 0)
			{
				std::cout << "error in New model" << std::endl;
				goto END;
			}
			m_CameraManager->NewCamera();

			m_timer->Tick();
			m_timer->Tick();

			float totTime = 0.0;
			while (totTime < 10.0)
			{
				loopTime.timeStart();
				totTime += m_timer->deltaTime();

				cleanTime.timeStart();
				renderer->Clean();
				cleanTime.timeEnd();

				for (int entityType = 0; entityType < m_EntityManager->GetEntityTypeAmount(); entityType++)
				{
					for (int entity = 0; entity < m_EntityManager->GetEntityAmount(entityType); entity++)
					{
						m_EntityManager->UpdateRot(XMFLOAT4(0, 1, 0, 0), m_timer->deltaTime(), entity, entityType);
					}
				}
				m_CameraManager->GetCamera(0)->MovingCamera(m_timer->deltaTime());
				m_EntityManager->Frame();
				updateTime.timeStart();
				m_CameraManager->Frame();			
				m_EntityManager->UpdateBuffers(0);
				updateTime.timeEnd();
				Time.timeStart();				
				renderer->Render(m_EntityManager->GetTransBufferIndex(0), m_ModelManager->GetModel(0)->GetMBufferIndex(), m_ModelManager->GetModel(0)->GetVBufferIndex(), m_ModelManager->GetModel(0)->GetTexIndex(), m_CameraManager->GetCamera(0)->GetCameraIndex(), m_ModelManager->GetFaceCount(0), m_EntityManager->GetEntityAmount(0));
				Time.timeEnd();
				presentTime.timeStart();
				renderer->Present();
				presentTime.timeEnd();
				m_timer->Tick();
				loopTime.timeEnd();
			}
		}
		delete m_CameraManager;
		delete m_ModelManager;
		delete m_EntityManager;
		delete m_timer;
		delete m_LightManager;
		delete renderer;
		window->Shutdown();
		delete window;
		
	}
	std::cout << "Init renderer: " << std::endl;
	InitTime.printTime();
	InitTime.cleanTime();
	std::cout << "Model Load: " << std::endl;
	modelTime.printTime();
	modelTime.cleanTime();
	std::cout << "Render: " << std::endl;
	Time.printTime();
	Time.cleanTime();
	std::cout << "Clean: " << std::endl;
	cleanTime.printTime();
	cleanTime.cleanTime();
	std::cout << "Present: " << std::endl;
	presentTime.printTime();
	presentTime.cleanTime();
	std::cout << "Update: " << std::endl;
	updateTime.printTime();
	updateTime.cleanTime();
	std::cout << "Loop: " << std::endl;
	loopTime.printTime();
	loopTime.cleanTime();
	
	bool quit = false;
	while (!quit)
	{
		if (GetAsyncKeyState(VK_END))
		{
			quit = true;
		}
	}

	return true;
END:
	getchar();
	return false;
}