//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Text/BsFontDesc.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	template<> struct RTTIPlainType<CharDesc>
	{
		enum { id = TID_CHAR_DESC }; enum { hasDynamicSize = 1 };

		static void toMemory(const CharDesc& data, char* memory)
		{
			UINT32 size = getDynamicSize(data);

			memcpy(memory, &size, sizeof(UINT32));
			memory += sizeof(UINT32);

			memory = rttiWriteElem(data.charId, memory);
			memory = rttiWriteElem(data.page, memory);
			memory = rttiWriteElem(data.uvX, memory);
			memory = rttiWriteElem(data.uvY, memory);
			memory = rttiWriteElem(data.uvWidth, memory);
			memory = rttiWriteElem(data.uvHeight, memory);
			memory = rttiWriteElem(data.width, memory);
			memory = rttiWriteElem(data.height, memory);
			memory = rttiWriteElem(data.xOffset, memory);
			memory = rttiWriteElem(data.yOffset, memory);
			memory = rttiWriteElem(data.xAdvance, memory);
			memory = rttiWriteElem(data.yAdvance, memory);
			rttiWriteElem(data.kerningPairs, memory);
		}

		static UINT32 fromMemory(CharDesc& data, char* memory)
		{
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32));
			memory += sizeof(UINT32);

			memory = rttiReadElem(data.charId, memory);
			memory = rttiReadElem(data.page, memory);
			memory = rttiReadElem(data.uvX, memory);
			memory = rttiReadElem(data.uvY, memory);
			memory = rttiReadElem(data.uvWidth, memory);
			memory = rttiReadElem(data.uvHeight, memory);
			memory = rttiReadElem(data.width, memory);
			memory = rttiReadElem(data.height, memory);
			memory = rttiReadElem(data.xOffset, memory);
			memory = rttiReadElem(data.yOffset, memory);
			memory = rttiReadElem(data.xAdvance, memory);
			memory = rttiReadElem(data.yAdvance, memory);
			rttiReadElem(data.kerningPairs, memory);

			return size;
		}

		static UINT32 getDynamicSize(const CharDesc& data)
		{
			UINT64 dataSize = sizeof(data.charId)
				+ sizeof(data.page)
				+ sizeof(data.uvX)
				+ sizeof(data.uvY)
				+ sizeof(data.uvWidth)
				+ sizeof(data.uvHeight)
				+ sizeof(data.width)
				+ sizeof(data.height)
				+ sizeof(data.xOffset)
				+ sizeof(data.yOffset)
				+ sizeof(data.xAdvance)
				+ sizeof(data.yAdvance)
				+ RTTIPlainType<Vector<KerningPair>>::getDynamicSize(data.kerningPairs);

			dataSize += sizeof(UINT32);

			return (UINT32)dataSize;
		}
	};

	/** @} */
	/** @endcond */
}
