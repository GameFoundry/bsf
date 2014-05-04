#include "CmD3D11GpuProgramManager.h"
#include "CmD3D11GpuProgram.h"

namespace BansheeEngine
{
	D3D11GpuProgramManager::D3D11GpuProgramManager(D3D11Device& device)
		:mDevice(device)
	{ }

	D3D11GpuProgramManager::~D3D11GpuProgramManager()
	{ }

	GpuProgramPtr D3D11GpuProgramManager::create(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile)
	{
		switch(gptype)
		{
		case GPT_VERTEX_PROGRAM:
			return cm_core_ptr<D3D11GpuVertexProgram, PoolAlloc>(new (cm_alloc<D3D11GpuVertexProgram, PoolAlloc>()) D3D11GpuVertexProgram(profile));
		case GPT_FRAGMENT_PROGRAM:
			return cm_core_ptr<D3D11GpuFragmentProgram, PoolAlloc>(new (cm_alloc<D3D11GpuFragmentProgram, PoolAlloc>()) D3D11GpuFragmentProgram(profile));
		case GPT_HULL_PROGRAM:
			return cm_core_ptr<D3D11GpuHullProgram, PoolAlloc>(new (cm_alloc<D3D11GpuHullProgram, PoolAlloc>()) D3D11GpuHullProgram(profile));
		case GPT_DOMAIN_PROGRAM:
			return cm_core_ptr<D3D11GpuDomainProgram, PoolAlloc>(new (cm_alloc<D3D11GpuDomainProgram, PoolAlloc>()) D3D11GpuDomainProgram(profile));
		case GPT_GEOMETRY_PROGRAM:
			return cm_core_ptr<D3D11GpuGeometryProgram, PoolAlloc>(new (cm_alloc<D3D11GpuGeometryProgram, PoolAlloc>()) D3D11GpuGeometryProgram(profile));
		}
		
		return nullptr;
	}
}