#include <Window\IWindow.h>

#ifdef _DEBUG
#pragma comment(lib, "WindowD.lib")
#else
#pragma comment(lib, "Window.lib")
#endif

int main()
{
	D3DPROJECT::WINDOW::InitializeInfo winInitInfo;
	D3DPROJECT::WINDOW::IWindow* window = D3DPROJECT::WINDOW::CreateNewWindow();
	window->Initialize(winInitInfo);

	getchar();

	window->Shutdown();
	return 0;
}