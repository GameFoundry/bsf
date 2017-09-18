//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "GLSL/BsGLSLProgramFactory.h"
#include "GLSL/BsGLSLGpuProgram.h"

namespace bs { namespace ct
{
    const String GLSLProgramFactory::LANGUAGE_NAME = "glsl";

    const String& GLSLProgramFactory::getLanguage() const
    {
        return LANGUAGE_NAME;
    }

	SPtr<GpuProgram> GLSLProgramFactory::create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
    {
		GLSLGpuProgram* prog = new (bs_alloc<GLSLGpuProgram>()) GLSLGpuProgram(desc, deviceMask);

		SPtr<GLSLGpuProgram> gpuProg = bs_shared_ptr<GLSLGpuProgram>(prog);
		gpuProg->_setThisPtr(gpuProg);

		return gpuProg;
    }

	SPtr<GpuProgram> GLSLProgramFactory::create(GpuProgramType type, GpuDeviceFlags deviceMask)
	{
		GPU_PROGRAM_DESC desc;
		desc.type = type;

		GLSLGpuProgram* prog = new (bs_alloc<GLSLGpuProgram>()) GLSLGpuProgram(desc, deviceMask);

		SPtr<GLSLGpuProgram> gpuProg = bs_shared_ptr<GLSLGpuProgram>(prog);
		gpuProg->_setThisPtr(gpuProg);

		return gpuProg;
	}
}}