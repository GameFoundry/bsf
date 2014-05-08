#include "CmGLSupport.h"
#include "CmGLTexture.h"
#include "GL/glew.h"

GLenum GLEWAPIENTRY glewContextInit(BansheeEngine::GLSupport *glSupport);

namespace BansheeEngine
{
    void GLSupport::initialiseExtensions()
    {
		glewContextInit(this);

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
		StringStream ext;
        String str;

        const GLubyte* pcExt = glGetString(GL_EXTENSIONS);
		assert(pcExt && "Problems getting GL extension string using glGetString");

        ext << pcExt;

        while(ext >> str)
        {
            extensionList.insert(str);
        }
    }

    bool GLSupport::checkMinGLVersion(const String& v) const
    {
        unsigned int first, second, third;
        unsigned int cardFirst, cardSecond, cardThird;
        if(v == mVersion)
            return true;

        String::size_type pos = v.find(".");
        if(pos == String::npos)
            return false;

        String::size_type pos1 = v.rfind(".");
        if(pos1 == String::npos)
            return false;

        first = ::atoi(v.substr(0, pos).c_str());
        second = ::atoi(v.substr(pos + 1, pos1 - (pos + 1)).c_str());
        third = ::atoi(v.substr(pos1 + 1, v.length()).c_str());

        pos = mVersion.find(".");
        if(pos == String::npos)
            return false;

        pos1 = mVersion.rfind(".");
        if(pos1 == String::npos)
            return false;

        cardFirst  = ::atoi(mVersion.substr(0, pos).c_str());
        cardSecond = ::atoi(mVersion.substr(pos + 1, pos1 - (pos + 1)).c_str());
        cardThird  = ::atoi(mVersion.substr(pos1 + 1, mVersion.length()).c_str());

        if(first <= cardFirst && second <= cardSecond && third <= cardThird)
          return true;

        return false;
    }

    bool GLSupport::checkExtension(const String& ext) const
    {
        if(extensionList.find(ext) == extensionList.end())
            return false; 
        
        return true;
    }
}
