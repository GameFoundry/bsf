#include "CmD3D11HLSLProgramFactory.h"
#include "CmD3D11GpuProgram.h"

namespace BansheeEngine
{
    const String D3D11HLSLProgramFactory::LANGUAGE_NAME = "hlsl";

    D3D11HLSLProgramFactory::D3D11HLSLProgramFactory()
    {
    }

    D3D11HLSLProgramFactory::~D3D11HLSLProgramFactory()
    {
    }

    const String& D3D11HLSLProgramFactory::getLanguage(void) const
    {
        return LANGUAGE_NAME;
    }

	GpuProgramPtr D3D11HLSLProgramFactory::create(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requireAdjacencyInfo)
    {
		switch (gptype)
		{
		case GPT_VERTEX_PROGRAM:
			return cm_core_ptr<D3D11GpuVertexProgram, PoolAlloc>(new (cm_alloc<D3D11GpuVertexProgram, PoolAlloc>()) 
				D3D11GpuVertexProgram(source, entryPoint, profile, includes, requireAdjacencyInfo));
		case GPT_FRAGMENT_PROGRAM:
			return cm_core_ptr<D3D11GpuFragmentProgram, PoolAlloc>(new (cm_alloc<D3D11GpuFragmentProgram, PoolAlloc>()) 
				D3D11GpuFragmentProgram(source, entryPoint, profile, includes, requireAdjacencyInfo));
		case GPT_HULL_PROGRAM:
			return cm_core_ptr<D3D11GpuHullProgram, PoolAlloc>(new (cm_alloc<D3D11GpuHullProgram, PoolAlloc>()) 
				D3D11GpuHullProgram(source, entryPoint, profile, includes, requireAdjacencyInfo));
		case GPT_DOMAIN_PROGRAM:
			return cm_core_ptr<D3D11GpuDomainProgram, PoolAlloc>(new (cm_alloc<D3D11GpuDomainProgram, PoolAlloc>()) 
				D3D11GpuDomainProgram(source, entryPoint, profile, includes, requireAdjacencyInfo));
		case GPT_GEOMETRY_PROGRAM:
			return cm_core_ptr<D3D11GpuGeometryProgram, PoolAlloc>(new (cm_alloc<D3D11GpuGeometryProgram, PoolAlloc>()) 
				D3D11GpuGeometryProgram(source, entryPoint, profile, includes, requireAdjacencyInfo));
		}

		return nullptr;
    }

	GpuProgramPtr D3D11HLSLProgramFactory::create()
	{
		return cm_core_ptr<D3D11GpuVertexProgram, PoolAlloc>(new (cm_alloc<D3D11GpuVertexProgram, PoolAlloc>())
			D3D11GpuVertexProgram("", "", GPP_NONE, nullptr, false));
	}
}