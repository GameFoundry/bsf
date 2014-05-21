#include "CmGpuProgram.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmRenderSystemCapabilities.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmAsyncOp.h"
#include "CmGpuParams.h"
#include "CmGpuProgInclude.h"
#include "CmGpuProgramManager.h"
#include "CmResources.h"
#include "CmGpuProgramRTTI.h"

namespace BansheeEngine
{
    GpuProgram::GpuProgram(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool isAdjacencyInfoRequired) 
		:mEntryPoint(entryPoint), mType(gptype), mIsCompiled(false),
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
		return cm_shared_ptr<GpuParams, PoolAlloc>(std::ref(mParametersDesc), false);
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