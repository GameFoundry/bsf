//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"
#include "Reflection/BsRTTIType.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Utility
	 *  @{
	 */

	template<> struct RTTIPlainType<RTTIFieldSchema>
	{
		enum { id = TID_RTTIFieldSchema }; enum { hasDynamicSize = 0 };

		static uint32_t toMemory(const RTTIFieldSchema& data, Bitstream& stream, const RTTIFieldInfo& fieldInfo, bool compress)
		{
			uint32_t size = 0;
			size += rtti_write(data.id, stream);
			size += rtti_write(data.type, stream);
			size += rtti_write(data.isArray, stream);
			size += rtti_write(data.hasDynamicSize, stream);
			size += rtti_write(data.size, stream);
			size += rtti_write(data.info, stream);
			
			return size;
		}

		static uint32_t fromMemory(RTTIFieldSchema& data, Bitstream& stream, const RTTIFieldInfo& fieldInfo, bool compress)
		{
			uint32_t size = 0;
			size += rtti_read(data.id, stream);
			size += rtti_read(data.type, stream);
			size += rtti_read(data.isArray, stream);
			size += rtti_read(data.hasDynamicSize, stream);
			size += rtti_read(data.size, stream);
			size += rtti_read(data.info, stream);
			
			return size;
		}

		static uint32_t getSize(const RTTIFieldSchema& data)
		{
			uint32_t size = 0;
			size += rtti_size(data.id);
			size += rtti_size(data.type);
			size += rtti_size(data.isArray);
			size += rtti_size(data.hasDynamicSize);
			size += rtti_size(data.size);
			size += rtti_size(data.info);
			
			return size;
		}
	};

	class RTTISchemaRTTI : public RTTIType<RTTISchema, IReflectable, RTTISchemaRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(typeId, 0)
			BS_RTTI_MEMBER_REFLPTR(baseTypeSchema, 1)
			BS_RTTI_MEMBER_PLAIN_ARRAY(fieldSchemas, 2)
		BS_END_RTTI_MEMBERS
		
	public:
		const String& getRTTIName() override
		{
			static String name = "RTTISchema";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_RTTISchema;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<RTTISchema>();
		}
	};

	/** @} */
	/** @endcond */
}
