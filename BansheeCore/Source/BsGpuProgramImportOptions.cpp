//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGpuProgramImportOptions.h"
#include "BsGpuProgramImportOptionsRTTI.h"

namespace BansheeEngine
{
	GpuProgramImportOptions::GpuProgramImportOptions()
		:mProfile(GPP_NONE), mType(GPT_VERTEX_PROGRAM)
	{

	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* GpuProgramImportOptions::getRTTIStatic()
	{
		return GpuProgramImportOptionsRTTI::instance();
	}

	RTTITypeBase* GpuProgramImportOptions::getRTTI() const
	{
		return GpuProgramImportOptions::getRTTIStatic();
	}
}