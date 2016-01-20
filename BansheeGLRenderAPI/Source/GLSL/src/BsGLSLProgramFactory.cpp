//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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
		GLSLGpuProgramCore* prog = new (bs_alloc<GLSLGpuProgramCore>()) GLSLGpuProgramCore(source, entryPoint, gptype, profile, requireAdjacency);

		SPtr<GLSLGpuProgramCore> gpuProg = bs_shared_ptr<GLSLGpuProgramCore>(prog);
		gpuProg->_setThisPtr(gpuProg);

		return gpuProg;
    }

	SPtr<GpuProgramCore> GLSLProgramFactory::create(GpuProgramType type)
	{
		GLSLGpuProgramCore* prog = new (bs_alloc<GLSLGpuProgramCore>()) GLSLGpuProgramCore("", "", type, GPP_NONE, false);

		SPtr<GLSLGpuProgramCore> gpuProg = bs_shared_ptr<GLSLGpuProgramCore>(prog);
		gpuProg->_setThisPtr(gpuProg);

		return gpuProg;
	}
}
