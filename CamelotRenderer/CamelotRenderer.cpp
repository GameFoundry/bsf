// CamelotRenderer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>

#include "CmColourValue.h"
#include "CmMath.h"
#include "CmMatrix3.h"
#include "CmMatrix4.h"
#include "CmPlane.h"
#include "CmPrerequisites.h"
#include "CmQuaternion.h"
#include "CmString.h"
#include "CmStringConverter.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmHardwareBuffer.h"

#include "CmD3D9Prerequisites.h"
#include "CmD3D9VideoMode.h"
#include "CmRenderSystem.h"

#include "CmApplication.h"

using namespace CamelotEngine;

int _tmain(int argc, _TCHAR* argv[])
{
	//const String& name = CamelotEngine::gApplication().getRenderSystem()->getName();

	gApplication().startUp("RenderSystemD3D9.dll");

	int a = 5;

	gApplication().shutDown();

	return 0;
}

