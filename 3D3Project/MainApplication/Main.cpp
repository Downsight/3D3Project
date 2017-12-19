#include <Window\IWindow.h>
#include "TestProgram.h"
#include <time.h>
#include <crtdbg.h>

#ifdef _DEBUG
#pragma comment(lib, "WindowD.lib")
#pragma comment(lib, "RendererD.lib")
#else
#pragma comment(lib, "Window.lib")
#pragma comment(lib, "Renderer.lib")
#endif

int main()
{
	srand(int(0));

	//FOR DEBUGGING PURPOSE
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(161);
	int hej = rand()%100;
	int hej2 = rand() % 100;
	int hej3 = rand() % 100;
	//Create, initialize and START the game
	TestProgram _Test;
	WPARAM test = _Test.RunTestProgram();
	return test;



	getchar();
	return 0;
}