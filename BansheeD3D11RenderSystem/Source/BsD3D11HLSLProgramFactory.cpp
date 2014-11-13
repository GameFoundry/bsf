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

	SPtr<GpuProgramCore> D3D11HLSLProgramFactory::create(const String& source, const String& entryPoint,
		GpuProgramType gptype, GpuProgramProfile profile, bool requireAdjacencyInfo)
    {
		switch (gptype)
		{
		case GPT_VERTEX_PROGRAM:
			return bs_shared_ptr<D3D11GpuVertexProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuVertexProgramCore, GenAlloc>())
				D3D11GpuVertexProgramCore(source, entryPoint, profile));
		case GPT_FRAGMENT_PROGRAM:
			return bs_shared_ptr<D3D11GpuFragmentProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuFragmentProgramCore, GenAlloc>())
				D3D11GpuFragmentProgramCore(source, entryPoint, profile));
		case GPT_HULL_PROGRAM:
			return bs_shared_ptr<D3D11GpuHullProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuHullProgramCore, GenAlloc>())
				D3D11GpuHullProgramCore(source, entryPoint, profile));
		case GPT_DOMAIN_PROGRAM:
			return bs_shared_ptr<D3D11GpuDomainProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuDomainProgramCore, GenAlloc>())
				D3D11GpuDomainProgramCore(source, entryPoint, profile));
		case GPT_GEOMETRY_PROGRAM:
			return bs_shared_ptr<D3D11GpuGeometryProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuGeometryProgramCore, GenAlloc>())
				D3D11GpuGeometryProgramCore(source, entryPoint, profile, requireAdjacencyInfo));
		}

		return nullptr;
    }

	SPtr<GpuProgramCore> D3D11HLSLProgramFactory::create(GpuProgramType type)
	{
		switch (type)
		{
		case GPT_VERTEX_PROGRAM:
			return bs_shared_ptr<D3D11GpuVertexProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuVertexProgramCore, GenAlloc>())
				D3D11GpuVertexProgramCore("", "", GPP_NONE));
		case GPT_FRAGMENT_PROGRAM:
			return bs_shared_ptr<D3D11GpuFragmentProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuFragmentProgramCore, GenAlloc>())
				D3D11GpuFragmentProgramCore("", "", GPP_NONE));
		case GPT_HULL_PROGRAM:
			return bs_shared_ptr<D3D11GpuHullProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuHullProgramCore, GenAlloc>())
				D3D11GpuHullProgramCore("", "", GPP_NONE));
		case GPT_DOMAIN_PROGRAM:
			return bs_shared_ptr<D3D11GpuDomainProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuDomainProgramCore, GenAlloc>())
				D3D11GpuDomainProgramCore("", "", GPP_NONE));
		case GPT_GEOMETRY_PROGRAM:
			return bs_shared_ptr<D3D11GpuGeometryProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuGeometryProgramCore, GenAlloc>())
				D3D11GpuGeometryProgramCore("", "", GPP_NONE, false));
		}

		return nullptr;
	}
}