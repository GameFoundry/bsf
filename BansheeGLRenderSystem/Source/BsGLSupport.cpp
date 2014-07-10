//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGLSupport.h"
#include "BsGLTexture.h"
#include "GL/glew.h"

GLenum GLEWAPIENTRY glewContextInit(BansheeEngine::GLSupport *glSupport);

namespace BansheeEngine
{
    void GLSupport::initializeExtensions()
    {
		glewContextInit(this);
		glGetError();

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
		int numExtensions = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

		for (int i = 0; i < numExtensions; i++)
		{
			extensionList.insert(String((char*)glGetStringi(GL_EXTENSIONS, i)));
		}
    }

    bool GLSupport::checkExtension(const String& ext) const
    {
        if(extensionList.find(ext) == extensionList.end())
            return false; 
        
        return true;
    }
}
