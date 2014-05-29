#include "BsD3D9HLSLProgramFactory.h"
#include "BsString.h"
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

	GpuProgramPtr D3D9HLSLProgramFactory::create(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requiresAdjacency)
    {
		if (gptype == GPT_VERTEX_PROGRAM)
		{
			D3D9GpuVertexProgram* prog = new (bs_alloc<D3D9GpuVertexProgram, PoolAlloc>()) 
				D3D9GpuVertexProgram(source, entryPoint, profile, includes);

			return bs_core_ptr<D3D9GpuVertexProgram, PoolAlloc>(prog);
		}
		else if (gptype == GPT_FRAGMENT_PROGRAM)
		{
			D3D9GpuFragmentProgram* prog = new (bs_alloc<D3D9GpuFragmentProgram, PoolAlloc>())
				D3D9GpuFragmentProgram(source, entryPoint, profile, includes);

			return bs_core_ptr<D3D9GpuFragmentProgram, PoolAlloc>(prog);
		}

		return nullptr;
    }

	GpuProgramPtr D3D9HLSLProgramFactory::create(GpuProgramType type)
	{
		if (type == GPT_VERTEX_PROGRAM)
		{
			D3D9GpuVertexProgram* prog = new (bs_alloc<D3D9GpuVertexProgram, PoolAlloc>())
				D3D9GpuVertexProgram("", "", GPP_NONE, nullptr);

			return bs_core_ptr<D3D9GpuVertexProgram, PoolAlloc>(prog);
		}
		else if (type == GPT_FRAGMENT_PROGRAM)
		{
			D3D9GpuFragmentProgram* prog = new (bs_alloc<D3D9GpuFragmentProgram, PoolAlloc>())
				D3D9GpuFragmentProgram("", "", GPP_NONE, nullptr);

			return bs_core_ptr<D3D9GpuFragmentProgram, PoolAlloc>(prog);
		}

		return nullptr;
	}
}
