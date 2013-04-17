#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmGpuProgram.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuProgramRTTI : public RTTIType<GpuProgram, Resource, GpuProgramRTTI>
	{
	private:
		CM_SETGET_MEMBER(mSize, UINT32, GpuProgram)
		CM_SETGET_MEMBER(mUUID, String, GpuProgram);

		CM_SETGET_MEMBER(mType, GpuProgramType, GpuProgram);
		CM_SETGET_MEMBER(mNeedsAdjacencyInfo, bool, GpuProgram);
		CM_SETGET_MEMBER(mEntryPoint, String, GpuProgram);
		CM_SETGET_MEMBER(mProfile, GpuProgramProfile, GpuProgram);
		CM_SETGET_MEMBER(mSource, String, GpuProgram);
		CM_SETGET_MEMBER(mSyntaxCode, String, GpuProgram);

	public:
		GpuProgramRTTI()
		{
			CM_ADD_PLAINFIELD(mSize, 0, GpuProgramRTTI)
			CM_ADD_PLAINFIELD(mUUID, 1, GpuProgramRTTI)

			CM_ADD_PLAINFIELD(mType, 2, GpuProgramRTTI)
			CM_ADD_PLAINFIELD(mNeedsAdjacencyInfo, 3, GpuProgramRTTI)
			CM_ADD_PLAINFIELD(mEntryPoint, 4, GpuProgramRTTI)
			CM_ADD_PLAINFIELD(mProfile, 5, GpuProgramRTTI)
			CM_ADD_PLAINFIELD(mSource, 6, GpuProgramRTTI)
			CM_ADD_PLAINFIELD(mSyntaxCode, 7, GpuProgramRTTI)
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			GpuProgram* gpuProgram = static_cast<GpuProgram*>(obj);
			gpuProgram->initialize();
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