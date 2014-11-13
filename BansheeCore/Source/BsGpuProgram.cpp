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
	GpuProgramProperties::GpuProgramProperties(const String& source, const String& entryPoint,
		GpuProgramType gptype, GpuProgramProfile profile)
		:mSource(source), mEntryPoint(entryPoint), mType(gptype), mProfile(profile)
	{ }
		
	GpuProgramCore::GpuProgramCore(const String& source, const String& entryPoint,
		GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired)
		: mProperties(source, entryPoint, gptype, profile), mIsCompiled(false),
		mNeedsAdjacencyInfo(isAdjacencyInfoRequired)
	{
		mParametersDesc = bs_shared_ptr<GpuParamDesc>();
	}

	bool GpuProgramCore::isSupported() const
    {
		if (!isRequiredCapabilitiesSupported())
			return false;

		RenderSystem* rs = BansheeEngine::RenderSystem::instancePtr();
		String profile = rs->getCapabilities()->gpuProgProfileToRSSpecificProfile(getProperties().getProfile());

		return rs->getCapabilities()->isShaderProfileSupported(profile);
    }

	bool GpuProgramCore::isRequiredCapabilitiesSupported() const
	{
		return true;
	}

	GpuParamsPtr GpuProgramCore::createParameters()
	{
		return bs_shared_ptr<GpuParams, PoolAlloc>(mParametersDesc, false);
	}

	GpuProgram::GpuProgram(const String& source, const String& entryPoint, const String& language,
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool isAdjacencyInfoRequired) 
		: mProperties(mergeWithIncludes(source, includes), entryPoint, gptype, profile), mLanguage(language),
		 mNeedsAdjacencyInfo(isAdjacencyInfoRequired)
    {

    }

	bool GpuProgram::isCompiled() const
	{
		return getCore()->isCompiled();
	}

	String GpuProgram::getCompileErrorMessage() const
	{
		return getCore()->getCompileErrorMessage();
	}

	GpuParamsPtr GpuProgram::createParameters()
	{
		return getCore()->createParameters();
	}

	GpuParamDescPtr GpuProgram::getParamDesc() const
	{
		return getCore()->getParamDesc();
	}

	SPtr<GpuProgramCore> GpuProgram::getCore() const
	{
		return std::static_pointer_cast<GpuProgramCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> GpuProgram::createCore() const
	{
		return GpuProgramCoreManager::instance().createInternal(mProperties.getSource(), mProperties.getEntryPoint(),
			mLanguage, mProperties.getType(), mProperties.getProfile(), mNeedsAdjacencyInfo);
	}

	String GpuProgram::mergeWithIncludes(const String& source, const Vector<HGpuProgInclude>* includes)
	{
		if (includes != nullptr)
		{
			StringStream stringStream;
			for (auto iter = includes->begin(); iter != includes->end(); ++iter)
			{
				if (*iter != nullptr)
				{
					stringStream << (*iter)->getString();
				}
			}

			stringStream << source;

			return stringStream.str();
		}
		else
		{
			return source;
		}
	}

	HGpuProgram GpuProgram::create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype,
		GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requiresAdjacency)
	{
		GpuProgramPtr programPtr = _createPtr(source, entryPoint, language, gptype, profile, includes, requiresAdjacency);

		return static_resource_cast<GpuProgram>(gResources()._createResourceHandle(programPtr));
	}

	GpuProgramPtr GpuProgram::_createPtr(const String& source, const String& entryPoint,
		const String& language, GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requiresAdjacency)
	{
		return GpuProgramManager::instance().create(source, entryPoint, language, gptype, profile, includes, requiresAdjacency);
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