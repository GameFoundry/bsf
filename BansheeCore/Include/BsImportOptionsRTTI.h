//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsImportOptions.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT ImportOptionsRTTI : public RTTIType<ImportOptions, IReflectable, ImportOptionsRTTI>
	{
	public:
		ImportOptionsRTTI()
		{
		}

		virtual const String& getRTTIName()
		{
			static String name = "ImportOptions";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ImportOptions;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<ImportOptions, PoolAlloc>();
		}
	};
}