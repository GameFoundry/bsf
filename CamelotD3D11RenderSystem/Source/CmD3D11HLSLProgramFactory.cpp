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

	HighLevelGpuProgramPtr D3D11HLSLProgramFactory::create(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile, const vector<GpuProgIncludeHandle>::type* includes)
    {
		D3D11HLSLProgram* prog = CM_NEW(D3D11HLSLProgram, PoolAlloc) D3D11HLSLProgram(source, entryPoint, sLanguageName, gptype, profile, includes);

        return HighLevelGpuProgramPtr(prog, &CoreObject::_deleteDelayed<D3D11HLSLProgram, PoolAlloc>);
    }

	HighLevelGpuProgramPtr D3D11HLSLProgramFactory::create()
	{
		D3D11HLSLProgram* prog = CM_NEW(D3D11HLSLProgram, PoolAlloc) D3D11HLSLProgram("", "", sLanguageName, GPT_VERTEX_PROGRAM, GPP_NONE, nullptr);

		return HighLevelGpuProgramPtr(prog, &CoreObject::_deleteDelayed<D3D11HLSLProgram, PoolAlloc>);
	}
}