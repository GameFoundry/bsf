// CamelotRenderer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>

#include "OgreBuildSettings.h"
#include "OgreColourValue.h"
#include "OgreConfig.h"
#include "OgreMath.h"
#include "OgreMatrix3.h"
#include "OgreMatrix4.h"
#include "OgrePlane.h"
#include "OgrePlatform.h"
#include "OgrePrerequisites.h"
#include "OgreQuaternion.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
#include "OgreThreadDefines.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreVector4.h"
#include "OgreHardwareBuffer.h"

#include "OgreD3D9Prerequisites.h"
#include "OgreD3D9VideoMode.h"
#include "OgreRenderSystem.h"

#include "CmApplication.h"

using namespace CamelotEngine;

int _tmain(int argc, _TCHAR* argv[])
{
	//const Ogre::String& name = CamelotEngine::gApplication().getRenderSystem()->getName();

	gApplication().startUp();

	int a = 5;

	gApplication().shutDown();

	return 0;
}

