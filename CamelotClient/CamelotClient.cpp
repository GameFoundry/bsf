// CamelotClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CmApplication.h"

using namespace CamelotEngine;

int _tmain(int argc, _TCHAR* argv[])
{
	gApplication().startUp("RenderSystemD3D9.dll");

	int a = 5;

	gApplication().shutDown();

	return 0;
}

