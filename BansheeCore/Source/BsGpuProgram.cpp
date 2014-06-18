#include "BsGpuProgram.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsRenderSystemCapabilities.h"
#include "BsException.h"
#include "BsRenderSystem.h"
#include "BsAsyncOp.h"
#include "BsGpuParams.h"
#include "BsGpuProgInclude.h"
#include "BsGpuProgramManager.h"
#include "BsResources.h"
#include "BsGpuProgramRTTI.h"

namespace BansheeEngine
{
    GpuProgram::GpuProgram(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool isAdjacencyInfoRequired) 
		:mEntryPoint(entryPoint), mType(gptype), mIsCompiled(false),
		mProfile(profile), mNeedsAdjacencyInfo(isAdjacencyInfoRequired)
    {
		mParametersDesc = bs_shared_ptr<GpuParamDesc>();

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
		if (!isRequiredCapabilitiesSupported())
			return false;

		RenderSystem* rs = BansheeEngine::RenderSystem::instancePtr();
		String profile = rs->getCapabilities()->gpuProgProfileToRSSpecificProfile(mProfile);

		return rs->getCapabilities()->isShaderProfileSupported(profile);
    }

	bool GpuProgram::isRequiredCapabilitiesSupported() const
	{
		return true;
	}

	GpuParamsPtr GpuProgram::createParameters()
	{
		return bs_shared_ptr<GpuParams, PoolAlloc>(mParametersDesc, false);
	}

    const String& GpuProgram::getLanguage() const
    {
        static const String language = "null";

        return language;
    }

	HGpuProgram GpuProgram::create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype,
		GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requiresAdjacency)
	{
		GpuProgramPtr programPtr = _createPtr(source, entryPoint, language, gptype, profile, includes);

		return static_resource_cast<GpuProgram>(gResources()._createResourceHandle(programPtr));
	}

	GpuProgramPtr GpuProgram::_createPtr(const String& source, const String& entryPoint,
		const String& language, GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requiresAdjacency)
	{
		return GpuProgramManager::instance().create(source, entryPoint, language, gptype, profile, includes);
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