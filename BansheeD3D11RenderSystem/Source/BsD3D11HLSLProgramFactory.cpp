//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D11HLSLProgramFactory.h"
#include "BsD3D11GpuProgram.h"

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
			return bs_core_ptr<D3D11GpuVertexProgram, PoolAlloc>(new (bs_alloc<D3D11GpuVertexProgram, PoolAlloc>()) 
				D3D11GpuVertexProgram(source, entryPoint, profile, includes));
		case GPT_FRAGMENT_PROGRAM:
			return bs_core_ptr<D3D11GpuFragmentProgram, PoolAlloc>(new (bs_alloc<D3D11GpuFragmentProgram, PoolAlloc>()) 
				D3D11GpuFragmentProgram(source, entryPoint, profile, includes));
		case GPT_HULL_PROGRAM:
			return bs_core_ptr<D3D11GpuHullProgram, PoolAlloc>(new (bs_alloc<D3D11GpuHullProgram, PoolAlloc>()) 
				D3D11GpuHullProgram(source, entryPoint, profile, includes));
		case GPT_DOMAIN_PROGRAM:
			return bs_core_ptr<D3D11GpuDomainProgram, PoolAlloc>(new (bs_alloc<D3D11GpuDomainProgram, PoolAlloc>()) 
				D3D11GpuDomainProgram(source, entryPoint, profile, includes));
		case GPT_GEOMETRY_PROGRAM:
			return bs_core_ptr<D3D11GpuGeometryProgram, PoolAlloc>(new (bs_alloc<D3D11GpuGeometryProgram, PoolAlloc>()) 
				D3D11GpuGeometryProgram(source, entryPoint, profile, includes, requireAdjacencyInfo));
		}

		return nullptr;
    }

	GpuProgramPtr D3D11HLSLProgramFactory::create(GpuProgramType type)
	{
		switch (type)
		{
		case GPT_VERTEX_PROGRAM:
			return bs_core_ptr<D3D11GpuVertexProgram, PoolAlloc>(new (bs_alloc<D3D11GpuVertexProgram, PoolAlloc>())
				D3D11GpuVertexProgram("", "", GPP_NONE, nullptr));
		case GPT_FRAGMENT_PROGRAM:
			return bs_core_ptr<D3D11GpuFragmentProgram, PoolAlloc>(new (bs_alloc<D3D11GpuFragmentProgram, PoolAlloc>())
				D3D11GpuFragmentProgram("", "", GPP_NONE, nullptr));
		case GPT_HULL_PROGRAM:
			return bs_core_ptr<D3D11GpuHullProgram, PoolAlloc>(new (bs_alloc<D3D11GpuHullProgram, PoolAlloc>())
				D3D11GpuHullProgram("", "", GPP_NONE, nullptr));
		case GPT_DOMAIN_PROGRAM:
			return bs_core_ptr<D3D11GpuDomainProgram, PoolAlloc>(new (bs_alloc<D3D11GpuDomainProgram, PoolAlloc>())
				D3D11GpuDomainProgram("", "", GPP_NONE, nullptr));
		case GPT_GEOMETRY_PROGRAM:
			return bs_core_ptr<D3D11GpuGeometryProgram, PoolAlloc>(new (bs_alloc<D3D11GpuGeometryProgram, PoolAlloc>())
				D3D11GpuGeometryProgram("", "", GPP_NONE, nullptr, false));
		}

		return nullptr;
	}
}