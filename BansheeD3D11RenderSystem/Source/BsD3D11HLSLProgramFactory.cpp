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
		SPtr<GpuProgramCore> gpuProg;

		switch (gptype)
		{
		case GPT_VERTEX_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuVertexProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuVertexProgramCore, GenAlloc>())
				D3D11GpuVertexProgramCore(source, entryPoint, profile));
			break;
		case GPT_FRAGMENT_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuFragmentProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuFragmentProgramCore, GenAlloc>())
				D3D11GpuFragmentProgramCore(source, entryPoint, profile));
			break;
		case GPT_HULL_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuHullProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuHullProgramCore, GenAlloc>())
				D3D11GpuHullProgramCore(source, entryPoint, profile));
			break;
		case GPT_DOMAIN_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuDomainProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuDomainProgramCore, GenAlloc>())
				D3D11GpuDomainProgramCore(source, entryPoint, profile));
			break;
		case GPT_GEOMETRY_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuGeometryProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuGeometryProgramCore, GenAlloc>())
				D3D11GpuGeometryProgramCore(source, entryPoint, profile, requireAdjacencyInfo));
			break;
		}

		if (gpuProg != nullptr)
			gpuProg->_setThisPtr(gpuProg);

		return gpuProg;
    }

	SPtr<GpuProgramCore> D3D11HLSLProgramFactory::create(GpuProgramType type)
	{
		SPtr<GpuProgramCore> gpuProg;

		switch (type)
		{
		case GPT_VERTEX_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuVertexProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuVertexProgramCore, GenAlloc>())
				D3D11GpuVertexProgramCore("", "", GPP_NONE));
			break;
		case GPT_FRAGMENT_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuFragmentProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuFragmentProgramCore, GenAlloc>())
				D3D11GpuFragmentProgramCore("", "", GPP_NONE));
			break;
		case GPT_HULL_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuHullProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuHullProgramCore, GenAlloc>())
				D3D11GpuHullProgramCore("", "", GPP_NONE));
			break;
		case GPT_DOMAIN_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuDomainProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuDomainProgramCore, GenAlloc>())
				D3D11GpuDomainProgramCore("", "", GPP_NONE));
			break;
		case GPT_GEOMETRY_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuGeometryProgramCore, GenAlloc>(new (bs_alloc<D3D11GpuGeometryProgramCore, GenAlloc>())
				D3D11GpuGeometryProgramCore("", "", GPP_NONE, false));
			break;
		}

		if (gpuProg != nullptr)
			gpuProg->_setThisPtr(gpuProg);

		return gpuProg;
	}
}