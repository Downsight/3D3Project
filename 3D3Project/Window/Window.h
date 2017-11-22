#pragma once
#ifndef _3D3Project_Window_H_
#define _3D3Project_Window_H_
#include "IWindow.h"
#include <SDL2/SDL.h>
#include <Windows.h>

namespace D3DPROJECT
{
	namespace WINDOW
	{
		class Window : public IWindow
		{
		public:
			Window();
			~Window();
			void Initialize(const InitializeInfo& info) override;
			void Shutdown() override;
		private:
			SDL_Window* window = nullptr;
			InitializeInfo InitInfo;
			HWND hwnd;
		};
	}
}


#endif