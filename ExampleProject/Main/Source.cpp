#include "stdafx.h"
#include <windows.h>

#include "BsApplication.h"
#include "CmRenderWindow.h"
#include "CmSceneObject.h"

using namespace BansheeEngine;

void setUpExample();

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	RENDER_WINDOW_DESC renderWindowDesc;
	renderWindowDesc.width = 1280;
	renderWindowDesc.height = 720;
	renderWindowDesc.title = "Banshee Example App";
	renderWindowDesc.fullscreen = false;

	gBansheeApp().startUp(renderWindowDesc, "CamelotD3D11RenderSystem", "BansheeForwardRenderer"); // TODO - Use enums instead of names. BansheeApp is a high level system that doesn't need to be as customizable.
	setUpExample();
	
	gBansheeApp().runMainLoop();
	gBansheeApp().shutDown();

	return 0;
}

void setUpExample()
{
	HSceneObject exampleSO = SceneObject::create("Example");



	// TODO - add ExampleGUI component
}