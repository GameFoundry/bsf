//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Allocators/BsMemoryAllocator.h"
#include "Prerequisites/BsFwdDeclUtil.h"      // For TIDs
#include "Prerequisites/BsTypes.h"            // For UINT32
#include "Utility/BsBitstream.h"

#include <limits>
#include <type_traits>          // For std::is_pod
#include <utility>              // For std::pair
#include <vector>

namespace bs
{
	/** @addtogroup Utility
	 *  @{
	 */

	/** @addtogroup RTTI
	 *  @{
	 */

	/**
	 * Template that you may specialize with a class if you want to provide simple serialization for it.
	 *
	 * Any type that uses the "plain" field in the RTTI system must specialize this class.
	 *
	 * @note
	 * Normally you will want to implement IReflectable interface if you want to provide serialization
	 * as that interface properly handles versioning, nested objects, pointer handling and more.
	 *
	 * @note
	 * This class is useful for types you can easily serialize using a memcpy (built-in types like int/float/etc), or
	 * types you cannot modify so they implement IReflectable interface (like std::string or std::vector).
	 *
	 * @see		RTTITypeBase
	 * @see		RTTIField
	 */
	template<class T>
	struct RTTIPlainType
	{
		static_assert(std::is_pod<T>::value,
			"Provided type isn't plain-old-data. You need to specialize RTTIPlainType template in order to serialize this type. "\
			" (Or call BS_ALLOW_MEMCPY_SERIALIZATION(type) macro if you are sure the type can be properly serialized using just memcpy.)");

		enum { id = 0 /**< Unique id for the serializable type. */ };
		enum { hasDynamicSize = 0 /**< 0 (Object has static size less than 255 bytes, for example int) or 1 (Dynamic size with no size restriction, for example string) */ };

		/** Serializes the provided object into the provided stream and advances the stream cursor. Returns the number of bytes written. */
		static uint32_t toMemory(const T& data, Bitstream& stream, const RTTIFieldInfo& fieldInfo)
		{
			return stream.writeBytes(data);
		}

		/** Deserializes a previously allocated object from the provided stream and advances the stream cursor. Return the number of bytes read. */
		static uint32_t fromMemory(T& data, Bitstream& stream, const RTTIFieldInfo& fieldInfo)
		{
			return stream.readBytes(data);
		}

		/** Returns the size of the provided object. (Works for both static and dynamic size types) */
		static UINT32 getDynamicSize(const T& data)
		{
			return sizeof(T);
		}
	};

	/**
	 * Helper method when serializing known data types that have valid
	 * RTTIPlainType specialization.
	 *
	 * Returns the size of the element. If elements serializable type is
	 * specialized with hasDynamicSize == true, the dynamic size is calculated,
	 * otherwise sizeof() is used.
	 */
	template<class ElemType>
	UINT32 rttiGetElemSize(const ElemType& data)
	{
		if(RTTIPlainType<ElemType>::hasDynamicSize == 1)
			return RTTIPlainType<ElemType>::getDynamicSize(data);
		else
			return sizeof(ElemType);
	}

	/**
	 * Helper method when serializing known data types that have valid RTTIPlainType specialization. The data will be
	 * written to the provided stream and its write cursor advanced.
	 */
	template<class ElemType>
	uint32_t rttiWriteElem(const ElemType& data, Bitstream& stream)
	{
		return RTTIPlainType<ElemType>::toMemory(data, stream, RTTIFieldInfo());
	}

	/**
	 * Helper method when serializing known data types that have valid RTTIPlainType specialization. The data will be
	 * read from the provided stream and its read cursor advanced.
	 */
	template<class ElemType>
	char* rttiReadElem(ElemType& data, Bitstream& stream)
	{
		return RTTIPlainType<ElemType>::fromMemory(data, stream, RTTIFieldInfo());
	}

	/** Helper for checking for existance of rttiEnumFields method on a class. */
	template <class T>
	struct has_rttiEnumFields
	{
		struct dummy {};

		template <typename C, typename P>
		static auto test(P* p) -> decltype(std::declval<C>().rttiEnumFields(*p), std::true_type());

		template <typename, typename>
		static std::false_type test(...);

		typedef decltype(test<T, dummy>(nullptr)) type;
		static const bool value = std::is_same<std::true_type, decltype(test<T, dummy>(nullptr))>::value;
	};

	/**
	 * Notify the RTTI system that the specified type may be serialized just by using a memcpy.
	 *
	 * @note	Internally this creates a basic RTTIPlainType<T> specialization for the type.
	 *
	 * @see		RTTIPlainType<T>
	 */
#define BS_ALLOW_MEMCPY_SERIALIZATION(type)																\
	static_assert (std::is_trivially_copyable<type>()==true,											\
						#type " is not trivially copyable");											\
	template<> struct RTTIPlainType<type>																\
	{	enum { id=0 }; enum { hasDynamicSize = 0 };														\
		static uint32_t toMemory(const type& data, Bitstream& stream, const RTTIFieldInfo& fieldInfo)	\
		{ return stream.writeBytes(data); }																\
		static uint32_t fromMemory(type& data, Bitstream& stream, const RTTIFieldInfo& fieldInfo)		\
		{ return stream.readBytes(data); }																\
		static UINT32 getDynamicSize(const type& data)													\
		{ return sizeof(type); }																		\
	};

	/** @} */
	/** @} */
}
