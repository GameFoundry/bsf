#pragma once

#include "CmPrerequisites.h"
#include "CmImportOptions.h"
#include "CmRTTIType.h"

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
			return cm_shared_ptr<ImportOptions, PoolAlloc>();
		}
	};
}