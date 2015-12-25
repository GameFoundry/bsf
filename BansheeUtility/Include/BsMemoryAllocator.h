#pragma once

#undef min
#undef max

#include <atomic>

/** @addtogroup Memory
 *  @{
 */

namespace BansheeEngine
{
	class MemoryAllocatorBase;

	/** @cond INTERNAL */

	/**
	 * Thread safe class used for storing total number of memory allocations and deallocations, primarily for statistic 
	 * purposes.
	 */
	class MemoryCounter
	{
	public:
		static BS_UTILITY_EXPORT UINT64 getNumAllocs()
		{
			return Allocs;
		}

		static BS_UTILITY_EXPORT UINT64 getNumFrees()
		{
			return Frees;
		}
		
	private:
		friend class MemoryAllocatorBase;

		// Threadlocal data can't be exported, so some magic to make it accessible from MemoryAllocator
		static BS_UTILITY_EXPORT void incAllocCount() { Allocs++; }
		static BS_UTILITY_EXPORT void incFreeCount() { Frees++; }

		static BS_THREADLOCAL UINT64 Allocs;
		static BS_THREADLOCAL UINT64 Frees;
	};

	/** Base class all memory allocators need to inherit. Provides allocation and free counting. */
	class MemoryAllocatorBase
	{
	protected:
		static void incAllocCount() { MemoryCounter::incAllocCount(); }
		static void incFreeCount() { MemoryCounter::incFreeCount(); }
	};

	/**
	 * Memory allocator providing a generic implementation. Specialize for specific categories as needed.
	 * 			
	 * @note	For example you might implement a pool allocator for specific types in order
	 * 			to reduce allocation overhead. By default standard malloc/free are used.
	 */
	template<class T>
	class MemoryAllocator : public MemoryAllocatorBase
	{
	public:
		static void* allocate(size_t bytes)
		{
#if BS_PROFILING_ENABLED
			incAllocCount();
#endif

			return malloc(bytes);
		}

		static void* allocateArray(size_t bytes, UINT32 count)
		{
#if BS_PROFILING_ENABLED
			incAllocCount();
#endif

			return malloc(bytes * count);
		}

		static void free(void* ptr)
		{
#if BS_PROFILING_ENABLED
			incFreeCount();
#endif

			::free(ptr);
		}

		static void freeArray(void* ptr, UINT32 count)
		{
#if BS_PROFILING_ENABLED
			incFreeCount();
#endif

			::free(ptr);
		}
	};

	/**
	 * General allocator provided by the OS. Use for persistent long term allocations, and allocations that don't 
	 * happen often.
	 */
	class GenAlloc
	{ };

	/** @endcond */

	/** Allocates the specified number of bytes. */
	template<class Alloc> 
	inline void* bs_alloc(UINT32 count)
	{
		return MemoryAllocator<Alloc>::allocate(count);
	}

	/** Allocates enough bytes to hold the specified type, but doesn't construct it. */
	template<class T, class Alloc> 
	inline T* bs_alloc()
	{
		return (T*)MemoryAllocator<Alloc>::allocate(sizeof(T));
	}

	/** Creates and constructs an array of @p count elements. */
	template<class T, class Alloc> 
	inline T* bs_newN(UINT32 count)
	{
		T* ptr = (T*)MemoryAllocator<Alloc>::allocateArray(sizeof(T), count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&ptr[i]) T;

		return ptr;
	}

	/** Create a new object with the specified allocator and the specified parameters. */
	template<class Type, class Alloc, class... Args>
	Type* bs_new(Args &&...args)
	{
		return new (bs_alloc<Alloc>(sizeof(Type))) Type(std::forward<Args>(args)...);
	}

	/** Frees all the bytes allocated at the specified location. */
	template<class Alloc> 
	inline void bs_free(void* ptr)
	{
		MemoryAllocator<Alloc>::free(ptr);
	}

	/** Destructs and frees the specified object. */
	template<class T, class Alloc = GenAlloc>
	inline void bs_delete(T* ptr)
	{
		(ptr)->~T();

		MemoryAllocator<Alloc>::free(ptr);
	}

	/** Destructs and frees the specified array of objects. */
	template<class T, class Alloc = GenAlloc>
	inline void bs_deleteN(T* ptr, UINT32 count)
	{
		for(unsigned int i = 0; i < count; i++)
			ptr[i].~T();

		MemoryAllocator<Alloc>::freeArray(ptr, count);
	}

	/*****************************************************************************/
	/* Default versions of all alloc/free/new/delete methods which call GenAlloc */
	/*****************************************************************************/

	/** Allocates the specified number of bytes. */
	inline void* bs_alloc(UINT32 count)
	{
		return MemoryAllocator<GenAlloc>::allocate(count);
	}

	/** Allocates enough bytes to hold the specified type, but doesn't construct it. */
	template<class T> 
	inline T* bs_alloc()
	{
		return (T*)MemoryAllocator<GenAlloc>::allocate(sizeof(T));
	}

	/** Allocates enough bytes to hold an array of @p count elements the specified type, but doesn't construct them. */
	template<class T> 
	inline T* bs_allocN(UINT32 count)
	{
		return (T*)MemoryAllocator<GenAlloc>::allocate(count * sizeof(T));
	}

	/** Creates and constructs an array of @p count elements. */
	template<class T> 
	inline T* bs_newN(UINT32 count)
	{
		T* ptr = (T*)MemoryAllocator<GenAlloc>::allocateArray(sizeof(T), count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&ptr[i]) T;

		return ptr;
	}

	/** Create a new object with the specified allocator and the specified parameters. */
	template<class Type, class... Args>
	Type* bs_new(Args &&...args)
	{
		return new (bs_alloc<GenAlloc>(sizeof(Type))) Type(std::forward<Args>(args)...);
	}

	/** Frees all the bytes allocated at the specified location. */
	inline void bs_free(void* ptr)
	{
		MemoryAllocator<GenAlloc>::free(ptr);
	}

/************************************************************************/
/* 							MACRO VERSIONS                      		*/
/* You will almost always want to use the template versions but in some */
/* cases (private destructor) it is not possible. In which case you may	*/
/* use these instead.												    */
/************************************************************************/
#define BS_PVT_DELETE(T, ptr) \
	(ptr)->~T(); \
	MemoryAllocator<GenAlloc>::free(ptr);

#define BS_PVT_DELETE_A(T, ptr, Alloc) \
	(ptr)->~T(); \
	MemoryAllocator<Alloc>::free(ptr);
}

namespace BansheeEngine
{
	/** @cond INTERNAL */

    /** Allocator for the standard library that internally uses Banshee memory allocator. */
    template <class T, class Alloc = GenAlloc>
	class StdAlloc 
	{
	public:
		typedef T value_type;
		StdAlloc() noexcept {}
		template<class T, class Alloc> StdAlloc(const StdAlloc<T, Alloc>&) noexcept {}
		template<class T, class Alloc> bool operator==(const StdAlloc<T, Alloc>&) const noexcept { return true; }
		template<class T, class Alloc> bool operator!=(const StdAlloc<T, Alloc>&) const noexcept { return false; }

		/** Allocate but don't initialize number elements of type T. */
		T* allocate(const size_t num) const
		{
			if (num == 0)
				return nullptr;

			if (num > static_cast<size_t>(-1) / sizeof(T))
				throw std::bad_array_new_length();

			void* const pv = bs_alloc<Alloc>((UINT32)(num * sizeof(T)));
			if (!pv)
				throw std::bad_alloc();

			return static_cast<T*>(pv);
		}

		/** Deallocate storage p of deleted elements. */
		void deallocate(T* p, size_t num) const noexcept
		{
			bs_free<Alloc>((void*)p);
		}
	};

	/** @endcond */
}

/** @} */

#include "BsMemStack.h"
#include "BsGlobalFrameAlloc.h"
#include "BsMemAllocProfiler.h"