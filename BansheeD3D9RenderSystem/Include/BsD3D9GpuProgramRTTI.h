//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsRTTIType.h"
#include "BsGpuProgramManager.h"
#include "BsD3D9GpuProgram.h"

namespace BansheeEngine
{
	class BS_D3D9_EXPORT D3D9GpuProgramRTTI : public RTTIType<D3D9GpuProgram, GpuProgram, D3D9GpuProgramRTTI>
	{
	public:
		D3D9GpuProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D9GpuProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D9_GpuProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			BS_EXCEPT(InvalidStateException, "Cannot create an instance of an abstract class.");
		}
	};

	class BS_D3D9_EXPORT D3D9GpuVertexProgramRTTI : public RTTIType<D3D9GpuVertexProgram, D3D9GpuProgram, D3D9GpuVertexProgramRTTI>
	{
	public:
		D3D9GpuVertexProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D9GpuVertexProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D9_GpuVertexProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgramManager::instance().createEmpty("hlsl", GPT_VERTEX_PROGRAM);
		}
	};

	class BS_D3D9_EXPORT D3D9GpuFragmentProgramRTTI : public RTTIType<D3D9GpuFragmentProgram, D3D9GpuProgram, D3D9GpuFragmentProgramRTTI>
	{
	public:
		D3D9GpuFragmentProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D9GpuFragmentProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D9_GpuFragmentProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgramManager::instance().createEmpty("hlsl", GPT_FRAGMENT_PROGRAM);
		}
	};
}