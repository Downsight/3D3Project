#pragma
#include "Window.h"
#include <assert.h>
#include <SDL2/SDL_syswm.h>
#include <exception>

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

D3DPROJECT::WINDOW::Window::Window()
{

}

D3DPROJECT::WINDOW::Window::~Window()
{

}

void D3DPROJECT::WINDOW::Window::Initialize(const InitializeInfo& info) 
{
	InitInfo = info;
	assert(SDL_Init(SDL_INIT_VIDEO) >= 0);

	uint32_t createFlags = SDL_WINDOW_SHOWN | (InitInfo.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	window = SDL_CreateWindow(InitInfo.windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, InitInfo.width, InitInfo.height, createFlags);

	assert(window != nullptr);

	SDL_SysWMinfo sdlInfo;
	SDL_VERSION(&sdlInfo.version);
	SDL_GetWindowWMInfo(window, &sdlInfo);
	hwnd = sdlInfo.info.win.window;

}

void D3DPROJECT::WINDOW::Window::Shutdown()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void* D3DPROJECT::WINDOW::Window::GetWindowHandle()
{
	return static_cast<void*>(hwnd);
}

int D3DPROJECT::WINDOW::Window::GetHeight()
{
	return InitInfo.height;
}

int D3DPROJECT::WINDOW::Window::GetWidth()
{
	return InitInfo.width;
}
