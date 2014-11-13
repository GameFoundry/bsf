#include "BsGLSLProgramFactory.h"
#include "BsGLSLGpuProgram.h"

namespace BansheeEngine 
{
    const String GLSLProgramFactory::LANGUAGE_NAME = "glsl";

    const String& GLSLProgramFactory::getLanguage() const
    {
        return LANGUAGE_NAME;
    }

	SPtr<GpuProgramCore> GLSLProgramFactory::create(const String& source, const String& entryPoint,
		GpuProgramType gptype, GpuProgramProfile profile, bool requireAdjacency)
    {
		GLSLGpuProgramCore* prog = new (bs_alloc<GLSLGpuProgramCore, GenAlloc>()) GLSLGpuProgramCore(source, entryPoint, gptype, profile, requireAdjacency);

		return bs_shared_ptr<GLSLGpuProgramCore, GenAlloc>(prog);
    }

	SPtr<GpuProgramCore> GLSLProgramFactory::create(GpuProgramType type)
	{
		GLSLGpuProgramCore* prog = new (bs_alloc<GLSLGpuProgramCore, GenAlloc>()) GLSLGpuProgramCore("", "", type, GPP_NONE, false);

		return bs_shared_ptr<GLSLGpuProgramCore, GenAlloc>(prog);
	}
}
