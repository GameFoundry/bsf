#pragma once

#include "CmGLPrerequisites.h"
#include "CmGpuProgramManager.h"

namespace BansheeEngine
{
    /** Factory class for GLSL programs. */
    class CM_RSGL_EXPORT GLSLProgramFactory : public GpuProgramFactory
    {
    public:
		/// Get the name of the language this factory creates programs for
		const String& getLanguage() const;
		/// create an instance of GLSLProgram
        GpuProgramPtr create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requireAdjacency);
		GpuProgramPtr create(GpuProgramType type);

	protected:
		static const String LANGUAGE_NAME;
    };
}