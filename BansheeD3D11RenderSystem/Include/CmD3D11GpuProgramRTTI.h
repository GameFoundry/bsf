#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmGpuProgramManager.h"
#include "CmD3D11GpuProgram.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11GpuProgramRTTI : public RTTIType<D3D11GpuProgram, GpuProgram, D3D11GpuProgramRTTI>
	{
	public:
		D3D11GpuProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D11GpuProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D11_GpuProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(InvalidStateException, "Cannot create an instance of an abstract class.");
		}
	};

	class BS_D3D11_EXPORT D3D11GpuVertexProgramRTTI : public RTTIType<D3D11GpuVertexProgram, D3D11GpuProgram, D3D11GpuVertexProgramRTTI>
	{
	public:
		D3D11GpuVertexProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D11GpuVertexProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D11_GpuVertexProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgramManager::instance().createEmpty("hlsl", GPT_VERTEX_PROGRAM);
		}
	};

	class BS_D3D11_EXPORT D3D11GpuFragmentProgramRTTI : public RTTIType<D3D11GpuFragmentProgram, D3D11GpuProgram, D3D11GpuFragmentProgramRTTI>
	{
	public:
		D3D11GpuFragmentProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D11GpuFragmentProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D11_GpuFragmentProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgramManager::instance().createEmpty("hlsl", GPT_FRAGMENT_PROGRAM);
		}
	};

	class BS_D3D11_EXPORT D3D11GpuGeometryProgramRTTI : public RTTIType<D3D11GpuGeometryProgram, D3D11GpuProgram, D3D11GpuGeometryProgramRTTI>
	{
	public:
		D3D11GpuGeometryProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D11GpuGeometryProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D11_GpuGeometryProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgramManager::instance().createEmpty("hlsl", GPT_GEOMETRY_PROGRAM);
		}
	};

	class BS_D3D11_EXPORT D3D11GpuHullProgramRTTI : public RTTIType<D3D11GpuHullProgram, D3D11GpuProgram, D3D11GpuHullProgramRTTI>
	{
	public:
		D3D11GpuHullProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D11GpuHullProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D11_GpuHullProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgramManager::instance().createEmpty("hlsl", GPT_HULL_PROGRAM);
		}
	};

	class BS_D3D11_EXPORT D3D11GpuDomainProgramRTTI : public RTTIType<D3D11GpuDomainProgram, D3D11GpuProgram, D3D11GpuDomainProgramRTTI>
	{
	public:
		D3D11GpuDomainProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D11GpuDomainProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D11_GpuDomainProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgramManager::instance().createEmpty("hlsl", GPT_DOMAIN_PROGRAM);
		}
	};

	class BS_D3D11_EXPORT D3D11GpuComputeProgramRTTI : public RTTIType<D3D11GpuComputeProgram, D3D11GpuProgram, D3D11GpuComputeProgramRTTI>
	{
	public:
		D3D11GpuComputeProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "D3D11GpuComputeProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_D3D11_GpuComputeProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgramManager::instance().createEmpty("hlsl", GPT_COMPUTE_PROGRAM);
		}
	};
}