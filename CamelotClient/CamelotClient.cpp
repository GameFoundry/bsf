// CamelotClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CmApplication.h"

#include "TestingGround.h"

using namespace CamelotEngine;

int _tmain(int argc, _TCHAR* argv[])
{
	test();

	gApplication().startUp("CamelotGLRenderer.dll");
	//gApplication().startUp("CamelotD3D9Renderer.dll");

	int a = 5;

	gApplication().shutDown();

	return 0;
}

