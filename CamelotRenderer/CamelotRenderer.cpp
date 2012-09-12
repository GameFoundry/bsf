// CamelotRenderer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>

#include "OgreBuildSettings.h"
#include "OgreColourValue.h"
#include "OgreConfig.h"
#include "CmMath.h"
#include "CmMatrix3.h"
#include "CmMatrix4.h"
#include "CmPlane.h"
#include "OgrePlatform.h"
#include "OgrePrerequisites.h"
#include "CmQuaternion.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
#include "OgreThreadDefines.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "OgreHardwareBuffer.h"

#include "OgreD3D9Prerequisites.h"
#include "OgreD3D9VideoMode.h"
#include "OgreRenderSystem.h"

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

