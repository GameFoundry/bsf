#include "BsGpuProgram.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsRenderAPICapabilities.h"
#include "BsException.h"
#include "BsRenderAPI.h"
#include "BsAsyncOp.h"
#include "BsGpuParams.h"
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

		RenderAPICore* rs = BansheeEngine::RenderAPICore::instancePtr();
		String profile = rs->getCapabilities()->gpuProgProfileToRSSpecificProfile(getProperties().getProfile());

		return rs->getCapabilities()->isShaderProfileSupported(profile);
    }

	bool GpuProgramCore::isRequiredCapabilitiesSupported() const
	{
		return true;
	}

	SPtr<GpuParamsCore> GpuProgramCore::createParameters()
	{
		return GpuParamsCore::create(mParametersDesc, false);
	}

	SPtr<GpuProgramCore> GpuProgramCore::create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype,
		GpuProgramProfile profile, bool requiresAdjacency)
	{
		return GpuProgramCoreManager::instance().create(source, entryPoint, language, gptype, profile, requiresAdjacency);
	}

	GpuProgram::GpuProgram(const String& source, const String& entryPoint, const String& language,
		GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired) 
		: mProperties(source, entryPoint, gptype, profile), mLanguage(language),
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
		return GpuParams::create(getCore()->getParamDesc(), getCore()->hasColumnMajorMatrices());
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

	GpuProgramPtr GpuProgram::create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype,
		GpuProgramProfile profile, bool requiresAdjacency)
	{
		return GpuProgramManager::instance().create(source, entryPoint, language, gptype, profile, requiresAdjacency);
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