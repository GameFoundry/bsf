#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	struct KerningPair
	{
		UINT32 otherCharId;
		INT32 amount;
	};

	struct CHAR_DESC
	{
		UINT32 charId;
		UINT32 page;
		float uvX, uvY;
		float uvWidth, uvHeight;
		UINT32 width, height;
		INT32 xOffset, yOffset;
		INT32 xAdvance, yAdvance;

		vector<KerningPair>::type kerningPairs;
	};

	struct FONT_DESC
	{
		map<UINT32, CHAR_DESC>::type characters;
	};

	// Make CHAR_DESC serializable
	template<> struct RTTIPlainType<CHAR_DESC>
	{	
		enum { id = TID_CHAR_DESC }; enum { hasDynamicSize = 1 };

		static void toMemory(const CHAR_DESC& data, char* memory)
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
			memory = rttiWriteElem(data.kerningPairs, memory);
		}

		static UINT32 fromMemory(CHAR_DESC& data, char* memory)
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
			memory = rttiReadElem(data.kerningPairs, memory);

			return size;
		}

		static UINT32 getDynamicSize(const CHAR_DESC& data)	
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
				+ RTTIPlainType<std::vector<KerningPair>>::getDynamicSize(data.kerningPairs);

			dataSize += sizeof(UINT32);

#if CM_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				CM_EXCEPT(InternalErrorException, "Data overflow! Size doesn't fit into 32 bits.");
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

	// Make FONT_DESC serializable
	template<> struct RTTIPlainType<FONT_DESC>
	{	
		enum { id = TID_FONT_DESC }; enum { hasDynamicSize = 1 };

		static void toMemory(const FONT_DESC& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			memcpy(memory, &size, sizeof(UINT32));
			memory += sizeof(UINT32);
			
			RTTIPlainType<std::map<UINT32, CHAR_DESC>>::toMemory(data.characters, memory);
		}

		static UINT32 fromMemory(FONT_DESC& data, char* memory)
		{ 
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);

			RTTIPlainType<std::map<UINT32, CHAR_DESC>>::fromMemory(data.characters, memory);

			return size;
		}

		static UINT32 getDynamicSize(const FONT_DESC& data)	
		{ 
			UINT64 dataSize = sizeof(UINT32);
			dataSize += RTTIPlainType<std::map<UINT32, CHAR_DESC>>::getDynamicSize(data.characters);


#if CM_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				CM_EXCEPT(InternalErrorException, "Data overflow! Size doesn't fit into 32 bits.");
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 
}