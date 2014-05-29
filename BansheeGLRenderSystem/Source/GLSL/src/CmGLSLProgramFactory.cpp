#include "CmGLSLProgramFactory.h"
#include "CmGLSLGpuProgram.h"

namespace BansheeEngine 
{
    const String GLSLProgramFactory::LANGUAGE_NAME = "glsl";

    const String& GLSLProgramFactory::getLanguage() const
    {
        return LANGUAGE_NAME;
    }

    GpuProgramPtr GLSLProgramFactory::create(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requireAdjacency)
    {
		GLSLGpuProgram* prog = new (bs_alloc<GLSLGpuProgram, PoolAlloc>()) GLSLGpuProgram(source, entryPoint, gptype, profile, includes, requireAdjacency);

		return bs_core_ptr<GLSLGpuProgram, PoolAlloc>(prog);
    }

	GpuProgramPtr GLSLProgramFactory::create(GpuProgramType type)
	{
		GLSLGpuProgram* prog = new (bs_alloc<GLSLGpuProgram, PoolAlloc>()) GLSLGpuProgram("", "", type, GPP_NONE, nullptr, false);

		return bs_core_ptr<GLSLGpuProgram, PoolAlloc>(prog);
	}
}
