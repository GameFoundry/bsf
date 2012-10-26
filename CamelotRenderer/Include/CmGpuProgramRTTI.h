#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmGpuProgram.h"

namespace CamelotEngine
{
	class CM_EXPORT GpuProgramRTTI : public RTTIType<GpuProgram, Resource, GpuProgramRTTI>
	{
	private:
		CM_SETGET_MEMBER(mSize, UINT32, GpuProgram)
		CM_SETGET_MEMBER(mUUID, String, GpuProgram);

	public:
		GpuProgramRTTI()
		{
			CM_ADD_PLAINFIELD(mSize, 0, GpuProgramRTTI)
			CM_ADD_PLAINFIELD(mUUID, 1, GpuProgramRTTI)
		}

		virtual const String& getRTTIName()
		{
			static String name = "GpuProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GpuProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(InternalErrorException, "Cannot instantiate abstract class!"); // TODO - Need to initialize this properly
		}
	};
}