#pragma once
#include <IWindow.h>
#include "Window.h"

D3DPROJECT::WINDOW::IWindow::IWindow()
{
}

D3DPROJECT::WINDOW::IWindow::~IWindow()
{
}

DECLDIR D3DPROJECT::WINDOW::IWindow * D3DPROJECT::WINDOW::CreateNewWindow()
{
	return new Window;
}
