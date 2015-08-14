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

		virtual const String& getRTTIName() override
		{
			static String name = "ImportOptions";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_ImportOptions;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ImportOptions>();
		}
	};
}