#pragma once

namespace CamelotFramework
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
	UINT32 rttiGetElemSize(const ElemType& data)
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
	char* rttiWriteElem(const ElemType& data, char* memory)
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

		static void toMemory(const T& data, char* memory)	
		{ 
			memcpy(memory, &data, sizeof(T)); 
		}

		static UINT32 fromMemory(T& data, char* memory)
		{
			memcpy(&data, memory, sizeof(T)); 
			return sizeof(T);
		}

		static UINT32 getDynamicSize(const T& data)
		{ 
			assert(false); 
			return sizeof(T);
		}
	};

#define CM_ALLOW_MEMCPY_SERIALIZATION(type)				\
	template<> struct RTTIPlainType<##type##>			\
	{	enum { id=0 }; enum { hasDynamicSize = 0 };		\
	static void toMemory(const type& data, char* memory)	\
	{ memcpy(memory, &data, sizeof(##type##)); }			\
	static UINT32 fromMemory(##type##& data, char* memory)	\
	{ memcpy(&data, memory, sizeof(##type##)); return sizeof(##type##); }			\
	static UINT32 getDynamicSize(const type& data)		\
	{ assert(false); return sizeof(##type##); }				\
	}; 

	// RTTI types for some standard classes
	template<class T> struct RTTIPlainType<std::vector<T, StdAlloc<T>>>
	{	
		enum { id = TID_STDVECTOR }; enum { hasDynamicSize = 1 };

		static void toMemory(const std::vector<T, StdAlloc<T>>& data, char* memory)
		{ 
			UINT32 size = (UINT32)data.size();

			memcpy(memory, &size, sizeof(UINT32));
			memory += sizeof(UINT32);

			for(auto iter = data.begin(); iter != data.end(); ++iter)
			{
				UINT32 elementSize = RTTIPlainType<T>::getDynamicSize(*iter);
				RTTIPlainType<T>::toMemory(*iter, memory);

				memory += elementSize;
			}
		}

		static UINT32 fromMemory(std::vector<T, StdAlloc<T>>& data, char* memory)
		{ 
			UINT32 size = 0;
			UINT32 numElements;
			memcpy(&numElements, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);
			size += sizeof(UINT32);

			for(UINT32 i = 0; i < numElements; i++)
			{
				T element;
				UINT32 elementSize = RTTIPlainType<T>::fromMemory(element, memory);
				data.push_back(element);

				memory += elementSize;
				size += elementSize;
			}

			return size;
		}

		static UINT32 getDynamicSize(const std::vector<T, StdAlloc<T>>& data)	
		{ 
			UINT64 dataSize = sizeof(UINT32);

			for(auto iter = data.begin(); iter != data.end(); ++iter)
				dataSize += RTTIPlainType<T>::getDynamicSize(*iter);		

			assert(dataSize <= std::numeric_limits<UINT32>::max());

			return (UINT32)dataSize;
		}	
	}; 

	template<class Key, class Value> struct RTTIPlainType<std::map<Key, Value, std::less<Key>, StdAlloc<std::pair<const Key, Value>>>>
	{	
		enum { id = TID_STDMAP }; enum { hasDynamicSize = 1 };

		static void toMemory(const std::map<Key, Value, std::less<Key>, StdAlloc<std::pair<const Key, Value>>>& data, char* memory)
		{ 
			UINT32 size = (UINT32)data.size();

			memcpy(memory, &size, sizeof(UINT32));
			memory += sizeof(UINT32);

			for(auto iter = data.begin(); iter != data.end(); ++iter)
			{
				UINT32 keySize = RTTIPlainType<Key>::getDynamicSize(iter->first);
				RTTIPlainType<Key>::toMemory(iter->first, memory);

				memory += keySize;

				UINT32 valueSize = RTTIPlainType<Value>::getDynamicSize(iter->second);
				RTTIPlainType<Value>::toMemory(iter->second, memory);

				memory += valueSize;
			}
		}

		static UINT32 fromMemory(std::map<Key, Value, std::less<Key>, StdAlloc<std::pair<const Key, Value>>>& data, char* memory)
		{ 
			UINT32 size = 0;
			UINT32 numElements;
			memcpy(&numElements, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);
			size += sizeof(UINT32);

			for(UINT32 i = 0; i < numElements; i++)
			{
				Key key;
				UINT32 keySize = RTTIPlainType<Key>::fromMemory(key, memory);
				memory += keySize;

				Value value;
				UINT32 valueSize = RTTIPlainType<Value>::fromMemory(value, memory);
				memory += valueSize;

				data[key] = value; 
				size += keySize + valueSize;
			}

			return size;
		}

		static UINT32 getDynamicSize(const std::map<Key, Value, std::less<Key>, StdAlloc<std::pair<const Key, Value>>>& data)	
		{ 
			UINT64 dataSize = sizeof(UINT32);

			for(auto iter = data.begin(); iter != data.end(); ++iter)
			{
				dataSize += RTTIPlainType<Key>::getDynamicSize(iter->first);		
				dataSize += RTTIPlainType<Value>::getDynamicSize(iter->second);
			}

			assert(dataSize <= std::numeric_limits<UINT32>::max());

			return (UINT32)dataSize;
		}	
	}; 

	template<class A, class B> struct RTTIPlainType<std::pair<A, B>>
	{	
		enum { id = TID_STDPAIR }; enum { hasDynamicSize = 1 };

		static void toMemory(const std::pair<A, B>& data, char* memory)
		{ 
			UINT32 firstSize = RTTIPlainType<A>::getDynamicSize(data.first);
			RTTIPlainType<A>::toMemory(data.first, memory);

			memory += firstSize;

			UINT32 secondSize = RTTIPlainType<B>::getDynamicSize(data.second);
			RTTIPlainType<B>::toMemory(data.second, memory);

			memory += secondSize;
		}

		static UINT32 fromMemory(std::pair<A, B>& data, char* memory)
		{ 
			UINT32 firstSize = RTTIPlainType<A>::fromMemory(data.first, memory);
			memory += firstSize;

			UINT32 secondSize = RTTIPlainType<B>::fromMemory(data.second, memory);
			memory += secondSize;

			return firstSize + secondSize;
		}

		static UINT32 getDynamicSize(const std::pair<A, B>& data)	
		{ 
			UINT64 dataSize = RTTIPlainType<A>::getDynamicSize(data.first);		
			dataSize += RTTIPlainType<B>::getDynamicSize(data.second);

			assert(dataSize <= std::numeric_limits<UINT32>::max());

			return (UINT32)dataSize;
		}	
	}; 
}