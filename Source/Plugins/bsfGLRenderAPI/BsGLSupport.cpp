//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsGLSupport.h"
#include "BsGLTexture.h"

#if BS_PLATFORM != BS_PLATFORM_OSX
#include "GL/glew.h"

GLenum glewContextInit(bs::ct::GLSupport* glSupport);
#endif

namespace bs { namespace ct
{
	void GLSupport::initializeExtensions()
	{
#if BS_PLATFORM != BS_PLATFORM_OSX
		glewContextInit(this);
		BS_CHECK_GL_ERROR();
#endif

		// Set version string
		const GLubyte* pcVer = glGetString(GL_VERSION);
		assert(pcVer && "Problems getting GL version string using glGetString");

		String tmpStr = (const char*)pcVer;
		mVersion = tmpStr.substr(0, tmpStr.find(" "));

		// Get vendor
		const GLubyte* pcVendor = glGetString(GL_VENDOR);
		tmpStr = (const char*)pcVendor;
		mVendor = tmpStr.substr(0, tmpStr.find(" "));

		// Set extension list
		INT32 numExtensions = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
		BS_CHECK_GL_ERROR();

		for (INT32 i = 0; i < numExtensions; i++)
			extensionList.insert(String((char*)glGetStringi(GL_EXTENSIONS, i)));
	}

	bool GLSupport::checkExtension(const String& ext) const
	{
		if(extensionList.find(ext) == extensionList.end())
			return false;

		return true;
	}
}}
