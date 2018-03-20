//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "GLSL/BsGLSLProgramFactory.h"
#include "GLSL/BsGLSLGpuProgram.h"

namespace bs { namespace ct
{
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

	GpuProgramCompileStatus GLSLProgramFactory::compile(const GPU_PROGRAM_DESC& desc)
	{
		// Note: No bytecode format for GLSL, so we just write the original source. Note that a SPIR-V extensions for GLSL
		// exists, and that could be used eventually (although support might be limited, esp. on macOS and mobiles).

		DataBlob blob;
		blob.size = 0;
		blob.size += rttiGetElemSize(desc.type);
		blob.size += rttiGetElemSize(desc.entryPoint);
		blob.size += rttiGetElemSize(desc.language);
		blob.size += rttiGetElemSize(desc.source);
		blob.size += rttiGetElemSize(desc.requiresAdjacency);

		blob.data = (UINT8*)bs_alloc(blob.size);

		char* memory = (char*)blob.data;
		memory = rttiWriteElem(desc.type, memory);
		memory = rttiWriteElem(desc.entryPoint, memory);
		memory = rttiWriteElem(desc.language, memory);
		memory = rttiWriteElem(desc.source, memory);
		memory = rttiWriteElem(desc.requiresAdjacency, memory);

		GpuProgramCompileStatus status;
		status.success = true;
		status.program.machineSpecific = false;
		status.program.instructions = blob;

		return status;
	}
}}