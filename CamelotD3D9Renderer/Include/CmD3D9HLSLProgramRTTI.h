#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmHighLevelGpuProgram.h"
#include "CmD3D9HLSLProgram.h"

namespace CamelotFramework
{
	class CM_D3D9_EXPORT D3D9HLSLProgramRTTI : public RTTIType<D3D9HLSLProgram, HighLevelGpuProgram, D3D9HLSLProgramRTTI>
	{
	public:
		D3D9HLSLProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D9HLSLProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D9_HLSLProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return HighLevelGpuProgramManager::instance().createEmpty("hlsl");
		}
	};
}