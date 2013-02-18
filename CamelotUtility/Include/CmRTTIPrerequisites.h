#pragma once

namespace CamelotEngine
{
	/**
	* @brief	Helper method when serializing known data types that have valid
	* 			RTTIPlainType specializations.
	* 			
	*			Returns the size of the element. If elements serializable type is 
	*			specialized with hasDynamicSize == true, the dynamic size is calculated, 
	*			otherwise sizeof() is used.
	 */
	template<class ElemType>
	UINT32 rttiGetElemSize(ElemType& data)
	{
		if(RTTIPlainType<ElemType>::hasDynamicSize == 1)
			return RTTIPlainType<ElemType>::getDynamicSize(data);
		else
			return sizeof(ElemType);
	}

	/**
	 * @brief	Helper method when serializing known data types that have valid
	 * 			RTTIPlainType specializations.
	 * 			
	 *			Writes the specified data into memory, advances the memory pointer by the
	 *			bytes written and returns pointer to new memory.
	 */
	template<class ElemType>
	char* rttiWriteElem(ElemType& data, char* memory)
	{
		RTTIPlainType<ElemType>::toMemory(data, memory);

		return memory + rttiGetElemSize(data);
	}

	/**
	 * @brief	Helper method when serializing known data types that have valid
	 * 			RTTIPlainType specializations.
	 * 			
	 *			Reads the specified data into memory, advances the memory pointer by the
	 *			bytes read and returns pointer to new memory.
	 */
	template<class ElemType>
	char* rttiReadElem(ElemType& data, char* memory)
	{
		RTTIPlainType<ElemType>::fromMemory(data, memory);

		return memory + rttiGetElemSize(data);
	}

	template<class T>
	struct RTTIPlainType 
	{ 
		static_assert(std::is_pod<T>::value, 
			"Provided type isn't plain-old-data. You need to specialize RTTIPlainType template in order to serialize this type. "\
			" (Or call CM_ALLOW_MEMCPY_SERIALIZATION(type) macro if you are sure the type can be properly serialized using just memcpy.)");

		enum { id = 0 }; 
		enum { hasDynamicSize = 0 };

		static void toMemory(T& data, char* memory)	
		{ 
			memcpy(memory, &data, sizeof(T)); 
		}

		static void fromMemory(T& data, char* memory)
		{
			memcpy(&data, memory, sizeof(T)); 
		}

		static UINT32 getDynamicSize(T& data)
		{ 
			assert(false); 
			return sizeof(T);
		}
	};

#define CM_ALLOW_MEMCPY_SERIALIZATION(type)				\
	template<> struct RTTIPlainType<##type##>			\
	{	enum { id=0 }; enum { hasDynamicSize = 0 };		\
	static void toMemory(##type##& data, char* memory)		\
	{ memcpy(memory, &data, sizeof(##type##)); }			\
	static void fromMemory(##type##& data, char* memory)	\
	{ memcpy(&data, memory, sizeof(##type##)); }			\
	static UINT32 getDynamicSize(##type##& data)			\
	{ assert(false); return sizeof(##type##); }				\
	}; 
}