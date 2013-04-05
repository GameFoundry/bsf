#include "CmD3D11GpuProgramManager.h"
#include "CmD3D11GpuProgram.h"

namespace CamelotEngine
{
	D3D11GpuProgramManager::D3D11GpuProgramManager(D3D11Device& device)
		:mDevice(device)
	{ }

	D3D11GpuProgramManager::~D3D11GpuProgramManager()
	{ }

	GpuProgramPtr D3D11GpuProgramManager::create(const String& source, const String& entryPoint, 
		const String& language, GpuProgramType gptype, GpuProgramProfile profile)
	{
		switch(gptype)
		{
		case GPT_VERTEX_PROGRAM:
			return GpuProgramPtr(CM_NEW(D3D11GpuVertexProgram, PoolAlloc) D3D11GpuVertexProgram(language), 
				&CoreObject::_deleteDelayed<D3D11GpuVertexProgram, PoolAlloc>);
		case GPT_FRAGMENT_PROGRAM:
			return GpuProgramPtr(CM_NEW(D3D11GpuFragmentProgram, PoolAlloc) D3D11GpuFragmentProgram(language), 
				&CoreObject::_deleteDelayed<D3D11GpuFragmentProgram, PoolAlloc>);
		case GPT_HULL_PROGRAM:
			return GpuProgramPtr(CM_NEW(D3D11GpuHullProgram, PoolAlloc) D3D11GpuHullProgram(language), 
				&CoreObject::_deleteDelayed<D3D11GpuHullProgram, PoolAlloc>);
		case GPT_DOMAIN_PROGRAM:
			return GpuProgramPtr(CM_NEW(D3D11GpuDomainProgram, PoolAlloc) D3D11GpuDomainProgram(language), 
				&CoreObject::_deleteDelayed<D3D11GpuDomainProgram, PoolAlloc>);
		case GPT_GEOMETRY_PROGRAM:
			return GpuProgramPtr(CM_NEW(D3D11GpuGeometryProgram, PoolAlloc) D3D11GpuGeometryProgram(language), 
				&CoreObject::_deleteDelayed<D3D11GpuGeometryProgram, PoolAlloc>);
		}
		
		return nullptr;
	}
}