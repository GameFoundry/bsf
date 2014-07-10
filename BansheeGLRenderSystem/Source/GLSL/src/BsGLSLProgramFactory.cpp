//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGLSLProgramFactory.h"
#include "BsGLSLGpuProgram.h"

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
