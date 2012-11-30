#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmHighLevelGpuProgram.h"
#include "CmGLSLProgram.h"

namespace CamelotEngine
{
	class CM_RSGL_EXPORT GLSLProgramRTTI : public RTTIType<GLSLProgram, HighLevelGpuProgram, GLSLProgramRTTI>
	{
	public:
		GLSLProgramRTTI()
		{ }

		virtual const String& getRTTIName()
		{
			static String name = "GLSLProgram";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GL_GLSLProgram;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return HighLevelGpuProgramManager::instance().create("glsl");
		}
	};
}