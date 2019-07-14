//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Importer/BsImportOptions.h"
#include "Reflection/BsRTTIType.h"

namespace bs
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

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ImportOptions>();
		}
	};

	/** @} */
	/** @endcond */
}
