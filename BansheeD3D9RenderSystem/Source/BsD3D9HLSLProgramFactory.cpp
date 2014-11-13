#include "BsD3D9HLSLProgramFactory.h"
#include "BsD3D9GpuProgram.h"

namespace BansheeEngine
{
    String D3D9HLSLProgramFactory::LANGUAGE_NAME = "hlsl";

    D3D9HLSLProgramFactory::D3D9HLSLProgramFactory()
    {
    }

    D3D9HLSLProgramFactory::~D3D9HLSLProgramFactory()
    {
    }

    const String& D3D9HLSLProgramFactory::getLanguage() const
    {
        return LANGUAGE_NAME;
    }

	SPtr<GpuProgramCore> D3D9HLSLProgramFactory::create(const String& source, const String& entryPoint,
		GpuProgramType gptype, GpuProgramProfile profile, bool requiresAdjacency)
    {
		if (gptype == GPT_VERTEX_PROGRAM)
		{
			D3D9GpuVertexProgramCore* prog = new (bs_alloc<D3D9GpuVertexProgramCore, GenAlloc>())
				D3D9GpuVertexProgramCore(source, entryPoint, profile);

			return bs_shared_ptr<D3D9GpuVertexProgramCore, GenAlloc>(prog);
		}
		else if (gptype == GPT_FRAGMENT_PROGRAM)
		{
			D3D9GpuFragmentProgramCore* prog = new (bs_alloc<D3D9GpuFragmentProgramCore, GenAlloc>())
				D3D9GpuFragmentProgramCore(source, entryPoint, profile);

			return bs_shared_ptr<D3D9GpuFragmentProgramCore, GenAlloc>(prog);
		}

		return nullptr;
    }

	SPtr<GpuProgramCore> D3D9HLSLProgramFactory::create(GpuProgramType type)
	{
		if (type == GPT_VERTEX_PROGRAM)
		{
			D3D9GpuVertexProgramCore* prog = new (bs_alloc<D3D9GpuVertexProgramCore, GenAlloc>())
				D3D9GpuVertexProgramCore("", "", GPP_NONE);

			return bs_shared_ptr<D3D9GpuVertexProgramCore, GenAlloc>(prog);
		}
		else if (type == GPT_FRAGMENT_PROGRAM)
		{
			D3D9GpuFragmentProgramCore* prog = new (bs_alloc<D3D9GpuFragmentProgramCore, GenAlloc>())
				D3D9GpuFragmentProgramCore("", "", GPP_NONE);

			return bs_shared_ptr<D3D9GpuFragmentProgramCore, GenAlloc>(prog);
		}

		return nullptr;
	}
}
