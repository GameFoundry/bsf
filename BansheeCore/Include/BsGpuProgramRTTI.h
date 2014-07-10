//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGpuProgram.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT GpuProgramRTTI : public RTTIType<GpuProgram, Resource, GpuProgramRTTI>
	{
	private:
		BS_SETGET_MEMBER(mSize, UINT32, GpuProgram)

		BS_SETGET_MEMBER(mType, GpuProgramType, GpuProgram);
		BS_SETGET_MEMBER(mNeedsAdjacencyInfo, bool, GpuProgram);
		BS_SETGET_MEMBER(mEntryPoint, String, GpuProgram);
		BS_SETGET_MEMBER(mProfile, GpuProgramProfile, GpuProgram);
		BS_SETGET_MEMBER(mSource, String, GpuProgram);

	public:
		GpuProgramRTTI()
		{
			BS_ADD_PLAINFIELD(mSize, 0, GpuProgramRTTI)

			BS_ADD_PLAINFIELD(mType, 2, GpuProgramRTTI)
			BS_ADD_PLAINFIELD(mNeedsAdjacencyInfo, 3, GpuProgramRTTI)
			BS_ADD_PLAINFIELD(mEntryPoint, 4, GpuProgramRTTI)
			BS_ADD_PLAINFIELD(mProfile, 5, GpuProgramRTTI)
			BS_ADD_PLAINFIELD(mSource, 6, GpuProgramRTTI)
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
			BS_EXCEPT(InternalErrorException, "Cannot instantiate abstract class!"); // TODO - Need to initialize this properly
		}
	};
}