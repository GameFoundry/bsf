//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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