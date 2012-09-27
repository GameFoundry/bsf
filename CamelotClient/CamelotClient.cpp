// CamelotClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CmApplication.h"
#include "CmDynLibManager.h"

#include "TestingGround.h"
#include "CmIReflectable.h"

using namespace CamelotEngine;

int _tmain(int argc, _TCHAR* argv[])
{
	//gApplication().startUp("CamelotGLRenderer.dll");
	gApplication().startUp("CamelotD3D9Renderer.dll");

	//RTTITypeBase* st = DbgSrlzTest::getRTTIStatic();

//	DbgSrlzTestST::startUp();
//	TextureST::startUp();

	test();

	gApplication().runMainLoop();

	int a = 5;

	gApplication().shutDown();

	return 0;
}

