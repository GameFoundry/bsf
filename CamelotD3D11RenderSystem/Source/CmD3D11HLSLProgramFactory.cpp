#include "CmD3D11HLSLProgramFactory.h"
#include "CmD3D11HLSLProgram.h"

namespace BansheeEngine
{
    String D3D11HLSLProgramFactory::sLanguageName = "hlsl";

    D3D11HLSLProgramFactory::D3D11HLSLProgramFactory()
    {
    }

    D3D11HLSLProgramFactory::~D3D11HLSLProgramFactory()
    {
    }

    const String& D3D11HLSLProgramFactory::getLanguage(void) const
    {
        return sLanguageName;
    }

	HighLevelGpuProgramPtr D3D11HLSLProgramFactory::create(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>::type* includes)
    {
		D3D11HLSLProgram* prog = new (cm_alloc<D3D11HLSLProgram, PoolAlloc>()) D3D11HLSLProgram(source, entryPoint, sLanguageName, gptype, profile, includes);

        return cm_core_ptr<D3D11HLSLProgram, PoolAlloc>(prog);
    }

	HighLevelGpuProgramPtr D3D11HLSLProgramFactory::create()
	{
		D3D11HLSLProgram* prog = new (cm_alloc<D3D11HLSLProgram, PoolAlloc>()) D3D11HLSLProgram("", "", sLanguageName, GPT_VERTEX_PROGRAM, GPP_NONE, nullptr);

		return cm_core_ptr<D3D11HLSLProgram, PoolAlloc>(prog);
	}
}