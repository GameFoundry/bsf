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