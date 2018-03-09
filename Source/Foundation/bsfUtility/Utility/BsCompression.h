//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"

namespace bs
{
	/** @addtogroup General
	 *  @{
	 */

	/** Performs generic compression and decompression on raw data. */
	class BS_UTILITY_EXPORT Compression
	{
	public:
		/** Compresses the data from the provided data stream and outputs the new stream with compressed data. */
		static SPtr<MemoryDataStream> compress(SPtr<DataStream>& input);

		/** Decompresses the data from the provided data stream and outputs the new stream with decompressed data. */
		static SPtr<MemoryDataStream> decompress(SPtr<DataStream>& input);
	};

	/** @} */
}