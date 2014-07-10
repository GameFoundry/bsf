//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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