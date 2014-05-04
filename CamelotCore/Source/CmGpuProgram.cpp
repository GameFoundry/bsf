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
    GpuProgram::GpuProgram(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>::type* includes, bool isAdjacencyInfoRequired) 
        :mEntryPoint(entryPoint), mType(gptype),
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

	GpuProgram::~GpuProgram()
	{
	}

    bool GpuProgram::isSupported() const
    {
		return false;
    }

	bool GpuProgram::isRequiredCapabilitiesSupported(void) const
	{
		return true;
	}

	GpuParamsPtr GpuProgram::createParameters(void)
	{
		return cm_shared_ptr<GpuParams, PoolAlloc>(std::ref(mParametersDesc), false);
	}

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