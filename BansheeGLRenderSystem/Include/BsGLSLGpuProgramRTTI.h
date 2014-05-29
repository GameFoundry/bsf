#pragma once

#include "BsGLPrerequisites.h"
#include "BsRTTIType.h"
#include "BsGpuProgramManager.h"
#include "BsGLSLGpuProgram.h"

namespace BansheeEngine
{
	class BS_RSGL_EXPORT GLSLGpuProgramRTTI : public RTTIType<GLSLGpuProgram, GpuProgram, GLSLGpuProgramRTTI>
	{
	public:
		GLSLGpuProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "GLSLGpuProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GL_GLSLGpuProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgramManager::instance().createEmpty("glsl", GPT_VERTEX_PROGRAM);
		}
	};
}