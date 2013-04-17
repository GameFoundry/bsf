#pragma once

namespace CamelotFramework
{
	/**
	 * @brief	Memory allocator providing a generic implementation. 
	 * 			Specialize for specific categories as needed.
	 */
	template<class T>
	class MemoryAllocator
	{
	public:
		static inline void* allocate(unsigned int bytes)
		{
			return malloc(bytes);
		}

		static inline void* allocateArray(unsigned int bytes, UINT32 count)
		{
			return malloc(bytes * count);
		}

		static inline void free(void* ptr)
		{
			::free(ptr);
		}

		static inline void freeArray(void* ptr, UINT32 count)
		{
			::free(ptr);
		}
	};

	template<class T, class category>
	class MemAllocDeleter
	{
	public:
		static void deleter(T* ptr)
		{
			__cm_destruct<T, category>(ptr);
		}
	};

	template<class category>
	class MemAllocBytesDeleter
	{
	public:
		static void deleter(void* ptr)
		{
			MemoryAllocator<category>::free(ptr);
		}
	};

	template<class T, class category> 
	inline T* __cm_construct_array(unsigned int count)
	{
		T* ptr = (T*)MemoryAllocator<category>::allocateArray(sizeof(T), count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&ptr[i]) T;

		return ptr;
	}

	template<class T, class category> 
	inline void __cm_destruct(T* ptr)
	{
		(ptr)->~T();

		MemoryAllocator<category>::free(ptr);
	}

	template<class T, class category> 
	inline void __cm_destruct_array(T* ptr, unsigned int count)
	{
		// This might seem a bit weird if T is a built-in type or a pointer, but
		// standard allows us to call destructor on such types (they don't do anything)
		for(unsigned int i = 0; i < count; i++)
			ptr[i].~T();

		MemoryAllocator<category>::freeArray(ptr, count);
	}

	class GenAlloc
	{ };

	/**
	 * @brief	Allocator used for allocating small amounts of temporary memory that
	 * 			used and then quickly released
	 */
	class ScratchAlloc
	{ };

	class PoolAlloc
	{ };
}

#define CM_NEW(T, category) new (MemoryAllocator<category>::allocate(sizeof(T)))
#define CM_NEW_BYTES(count, category) (UINT8*)MemoryAllocator<category>::allocate(count)
#define CM_NEW_ARRAY(T, count, category) __cm_construct_array<T, category>(count)
#define CM_DELETE(ptr, T, category) {(ptr)->~T(); MemoryAllocator<category>::free(ptr);}
#define CM_DELETE_BYTES(ptr, category) MemoryAllocator<category>::free(ptr)
#define CM_DELETE_ARRAY(ptr, T, count, category) __cm_destruct_array<T, category>(ptr, count)


