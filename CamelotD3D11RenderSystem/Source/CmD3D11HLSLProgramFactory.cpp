#include "CmD3D11HLSLProgramFactory.h"
#include "CmD3D11HLSLProgram.h"

namespace CamelotEngine
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

	HighLevelGpuProgram* D3D11HLSLProgramFactory::create(const String& source, const String& entryPoint, GpuProgramType gptype, GpuProgramProfile profile)
    {
		D3D11HLSLProgram* prog = new D3D11HLSLProgram(source, entryPoint, sLanguageName, gptype, profile);

        return prog;
    }

	HighLevelGpuProgram* D3D11HLSLProgramFactory::create()
	{
		D3D11HLSLProgram* prog = new D3D11HLSLProgram("", "", sLanguageName, GPT_VERTEX_PROGRAM, GPP_NONE);

		return prog;
	}

	void D3D11HLSLProgramFactory::destroy_internal(HighLevelGpuProgram* prog)
    {
        delete prog;
    }
}