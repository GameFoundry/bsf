#pragma once

#include "BsCorePrerequisites.h"
#include "BsImportOptions.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT ImportOptionsRTTI : public RTTIType<ImportOptions, IReflectable, ImportOptionsRTTI>
	{
	public:
		ImportOptionsRTTI()
		{ }

		const String& getRTTIName() override
		{
			static String name = "ImportOptions";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ImportOptions;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ImportOptions>();
		}
	};

	/** @} */
	/** @endcond */
}