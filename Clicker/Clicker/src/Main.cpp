//-----------------------------------------------------------------------------
//Author: Tommas Solarino
//Description: This is the main function
//-----------------------------------------------------------------------------
#include "Application.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application *app = new Application();
	app->Run();
	delete app;
}