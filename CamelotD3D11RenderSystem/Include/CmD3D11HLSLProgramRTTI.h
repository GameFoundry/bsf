#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmHighLevelGpuProgram.h"
#include "CmD3D11HLSLProgram.h"

namespace BansheeEngine
{
	class CM_D3D11_EXPORT D3D11HLSLProgramRTTI : public RTTIType<D3D11HLSLProgram, HighLevelGpuProgram, D3D11HLSLProgramRTTI>
	{
	public:
		D3D11HLSLProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D11HLSLProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D11_HLSLProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return HighLevelGpuProgramManager::instance().createEmpty("hlsl");
		}
	};
}