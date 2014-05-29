#include "BsImportOptions.h"
#include "BsImportOptionsRTTI.h"

namespace BansheeEngine
{
	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* ImportOptions::getRTTIStatic()
	{
		return ImportOptionsRTTI::instance();
	}

	RTTITypeBase* ImportOptions::getRTTI() const
	{
		return ImportOptions::getRTTIStatic();
	}
}