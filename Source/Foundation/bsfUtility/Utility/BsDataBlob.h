//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"
#include "Error/BsException.h"

namespace bs
{
	/** @addtogroup General
	 *  @{
	 */

	/** Serializable blob of raw memory. */
	struct DataBlob
	{
		UINT8* data = nullptr;
		UINT32 size = 0;
	};

	/** @} */
}
