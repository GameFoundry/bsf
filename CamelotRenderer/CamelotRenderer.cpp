// CamelotRenderer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>

#include "OgreColourValue.h"
#include "CmMath.h"
#include "CmMatrix3.h"
#include "CmMatrix4.h"
#include "CmPlane.h"
#include "OgrePrerequisites.h"
#include "CmQuaternion.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
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

	gApplication().startUp();

	int a = 5;

	gApplication().shutDown();

	return 0;
}

