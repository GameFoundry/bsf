#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmHighLevelGpuProgram.h"
#include "CmCgProgram.h"

namespace CamelotFramework
{
	class CM_EXPORT CgProgramRTTI : public RTTIType<CgProgram, HighLevelGpuProgram, CgProgramRTTI>
	{
	public:
		CgProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "CgProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_CgProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return HighLevelGpuProgramManager::instance().createEmpty("cg");
		}
	};
}