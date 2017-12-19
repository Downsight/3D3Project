#pragma once
#ifndef _3D3PROJECT_IWINDOW_H_
#define _3D3PROJECT_IWINDOW_H_
#include <string>

#if defined DLL_EXPORT_W
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

namespace D3DPROJECT
{
	namespace WINDOW
	{
		struct InitializeInfo
		{
			size_t height = 720;
			size_t width = 1280;
			bool fullscreen = false;
			std::string windowTitle = "NoTitle";
		};

		class IWindow
		{
		public:
			IWindow();
			virtual ~IWindow() = 0;
			virtual void Initialize(const InitializeInfo& info) = 0;
			virtual void Shutdown() = 0;
			virtual void* GetWindowHandle() = 0;
			virtual int GetHeight() = 0;
			virtual int GetWidth() = 0;
		private:

		};

		DECLDIR IWindow* CreateNewWindow();
	}
}


#endif