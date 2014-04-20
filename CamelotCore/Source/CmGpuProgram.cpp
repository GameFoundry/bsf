/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "CmGpuProgram.h"
#include "CmHighLevelGpuProgram.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmRenderSystemCapabilities.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmAsyncOp.h"
#include "CmGpuParams.h"
#include "CmGpuProgInclude.h"
#include "CmGpuProgramRTTI.h"

namespace BansheeEngine
{
    //-----------------------------------------------------------------------------
    GpuProgram::GpuProgram(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>::type* includes, bool isAdjacencyInfoRequired) 
        :mEntryPoint(entryPoint), mSyntaxCode(language), mType(gptype),
		mProfile(profile), mNeedsAdjacencyInfo(isAdjacencyInfoRequired)
    {
		if(includes != nullptr)
		{
			StringStream stringStream;
			for(auto iter = includes->begin(); iter != includes->end(); ++iter)
			{
				if(*iter != nullptr)
				{
					stringStream << (*iter)->getString();
				}
			}

			stringStream << source;

			mSource = stringStream.str();
		}
		else
		{
			mSource = source;
		}
    }
	//----------------------------------------------------------------------------
	GpuProgram::~GpuProgram()
	{
	}
    //-----------------------------------------------------------------------------
    bool GpuProgram::isSupported(void) const
    {
        if (!isRequiredCapabilitiesSupported())
            return false;

		RenderSystem* rs = BansheeEngine::RenderSystem::instancePtr();
		return rs->getCapabilities()->isShaderProfileSupported(mSyntaxCode);
    }
	//-----------------------------------------------------------------------------
	bool GpuProgram::isRequiredCapabilitiesSupported(void) const
	{
		return true;
	}
	//---------------------------------------------------------------------
	GpuParamsPtr GpuProgram::createParameters(void)
	{
		return cm_shared_ptr<GpuParams, PoolAlloc>(std::ref(mParametersDesc), false);
	}
    //-----------------------------------------------------------------------
    const String& GpuProgram::getLanguage(void) const
    {
        static const String language = "asm";

        return language;
    }

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* GpuProgram::getRTTIStatic()
	{
		return GpuProgramRTTI::instance();
	}

	RTTITypeBase* GpuProgram::getRTTI() const
	{
		return GpuProgram::getRTTIStatic();
	}
}