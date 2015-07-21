#pragma once

#undef min
#undef max

#include <atomic>

namespace BansheeEngine
{
	class MemoryAllocatorBase;

	/**
	 * @brief	Thread safe class used for storing total number of memory allocations and deallocations,
	 * 			primarily for statistic purposes.
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

	/**
	 * @brief	Base class all memory allocators need to inherit. Provides
	 * 			allocation and free counting.
	 */
	class MemoryAllocatorBase
	{
	protected:
		static void incAllocCount() { MemoryCounter::incAllocCount(); }
		static void incFreeCount() { MemoryCounter::incFreeCount(); }
	};

	/**
	 * @brief	Memory allocator providing a generic implementation. 
	 * 			Specialize for specific categories as needed.
	 * 			
	 * @note	For example you might implement a pool allocator for specific types in order
	 * 			to reduce allocation overhead. By default standard malloc/free are used.
	 */
	template<class T>
	class MemoryAllocator : public MemoryAllocatorBase
	{
	public:
		static inline void* allocate(size_t bytes)
		{
#if BS_PROFILING_ENABLED
			incAllocCount();
#endif

			return malloc(bytes);
		}

		static inline void* allocateArray(size_t bytes, UINT32 count)
		{
#if BS_PROFILING_ENABLED
			incAllocCount();
#endif

			return malloc(bytes * count);
		}

		static inline void free(void* ptr)
		{
#if BS_PROFILING_ENABLED
			incFreeCount();
#endif

			::free(ptr);
		}

		static inline void freeArray(void* ptr, UINT32 count)
		{
#if BS_PROFILING_ENABLED
			incFreeCount();
#endif

			::free(ptr);
		}
	};

	/**
	 * @brief	General allocator provided by the OS. Use for persistent long term allocations,
	 * 			and allocations that don't happen often.
	 */
	class GenAlloc
	{ };

	/**
	 * @brief	Allocator used for allocating small amounts of temporary memory that
	 * 			used and then quickly released
	 * 			
	 * @note	Currently not used.
	 */
	class ScratchAlloc
	{ };

	/**
	 * @brief	Pool allocator that is only suited for allocating one specific type of data. Most useful when you are
	 * 			often allocating one certain data type, with no specific allocation or deallocation order.
	 * 			
	 * @note	Currently not used.
	 */
	class PoolAlloc
	{ };

	/**
	 * @brief	Allocates the specified number of bytes.
	 */
	template<class Alloc> 
	inline void* bs_alloc(size_t count)
	{
		return MemoryAllocator<Alloc>::allocate(count);
	}

	/**
	 * @brief	Allocates enough bytes to hold the specified type, but doesn't construct it.
	 */
	template<class T, class Alloc> 
	inline T* bs_alloc()
	{
		return (T*)MemoryAllocator<Alloc>::allocate(sizeof(T));
	}

	/**
	 * @brief	Creates and constructs an array of "count" elements.
	 */
	template<class T, class Alloc> 
	inline T* bs_newN(UINT32 count)
	{
		T* ptr = (T*)MemoryAllocator<Alloc>::allocateArray(sizeof(T), count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&ptr[i]) T;

		return ptr;
	}

	/**
	 * @brief	Create a new object with the specified allocator and the specified parameters.
	 */
	template<class Type, class Alloc, class... Args>
	Type* bs_new(Args &&...args)
	{
		return new (bs_alloc<Alloc>(sizeof(Type))) Type(std::forward<Args>(args)...);
	}

	/**
	 * @brief	Frees all the bytes allocated at the specified location.
	 */
	template<class Alloc> 
	inline void bs_free(void* ptr)
	{
		MemoryAllocator<Alloc>::free(ptr);
	}

	/**
	 * @brief	Destructs and frees the specified object.
	 */
	template<class Alloc, class T> 
	inline void bs_delete(T* ptr)
	{
		(ptr)->~T();

		MemoryAllocator<Alloc>::free(ptr);
	}

	/**
	 * @brief	Destructs and frees the specified array of objects.
	 */
	template<class Alloc, class T> 
	inline void bs_deleteN(T* ptr, UINT32 count)
	{
		for(unsigned int i = 0; i < count; i++)
			ptr[i].~T();

		MemoryAllocator<Alloc>::freeArray(ptr, count);
	}

	/*****************************************************************************/
	/* Default versions of all alloc/free/new/delete methods which call GenAlloc */
	/*****************************************************************************/

	/**
	 * @brief	Allocates the specified number of bytes.
	 */
	inline void* bs_alloc(size_t count)
	{
		return MemoryAllocator<GenAlloc>::allocate(count);
	}

	/**
	 * @brief	Allocates enough bytes to hold the specified type, but doesn't construct it.
	 */
	template<class T> 
	inline T* bs_alloc()
	{
		return (T*)MemoryAllocator<GenAlloc>::allocate(sizeof(T));
	}

	/**
	 * @brief	Creates and constructs an array of "count" elements.
	 */
	template<class T> 
	inline T* bs_newN(UINT32 count)
	{
		T* ptr = (T*)MemoryAllocator<GenAlloc>::allocateArray(sizeof(T), count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&ptr[i]) T;

		return ptr;
	}

	/**
	 * @brief	Create a new object with the specified allocator and the specified parameters.
	 */
	template<class Type, class... Args>
	Type* bs_new(Args &&...args)
	{
		return new (bs_alloc<GenAlloc>(sizeof(Type))) Type(std::forward<Args>(args)...);
	}

	/**
	 * @brief	Frees all the bytes allocated at the specified location.
	 */
	inline void bs_free(void* ptr)
	{
		MemoryAllocator<GenAlloc>::free(ptr);
	}

	/**
	 * @brief	Destructs and frees the specified object.
	 */
	template<class T> 
	inline void bs_delete(T* ptr)
	{
		(ptr)->~T();

		MemoryAllocator<GenAlloc>::free(ptr);
	}

	/**
	 * @brief	Destructs and frees the specified array of objects.
	 */
	template<class T> 
	inline void bs_deleteN(T* ptr, UINT32 count)
	{
		for(unsigned int i = 0; i < count; i++)
			ptr[i].~T();

		MemoryAllocator<GenAlloc>::freeArray(ptr, count);
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
    /**
     * @brief	Allocator for the standard library that internally uses Banshee
     * 			memory allocator.
     */
    template <class T, class Alloc = GenAlloc>
	class StdAlloc 
	{
	public:
		// Type definitions
		typedef T        value_type;
		typedef T*       pointer;
		typedef const T* const_pointer;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef std::size_t    size_type;
		typedef std::ptrdiff_t difference_type;

		/**
		 * @brief	Rebind allocator to type U
		 */
		template <class U>
		struct rebind 
		{
			typedef StdAlloc<U, Alloc> other;
		};

		StdAlloc() throw() 
		{ }

		StdAlloc(const StdAlloc&) throw() 
		{ }

		template <class U>
		StdAlloc (const StdAlloc<U, Alloc>&) throw() 
		{ }

		~StdAlloc() throw() 
		{ }

		/**
		 * @brief	Return address of value.
		 */
		pointer address (reference value) const 
		{
			return &value;
		}

		/**
		 * @brief	Return address of value.
		 */
		const_pointer address (const_reference value) const 
		{
			return &value;
		}

		/**
		 * @brief	Return maximum number of elements that can be allocated.
		 */
		size_type max_size () const throw() 
		{
			return std::numeric_limits<std::size_t>::max() / sizeof(T);
		}

		/**
		 * @brief	Allocate but don't initialize number elements of type T.
		 */
		pointer allocate (size_type num, const void* = 0) 
		{
			pointer ret = (pointer)(bs_alloc<Alloc>((size_t)num*sizeof(T)));
			return ret;
		}

		/**
		 * @brief	Initialize elements of allocated storage p with value "value".
		 */
		void construct (pointer p, const T& value) 
		{
			new((void*)p)T(value);
		}

		/**
		 * @brief	Destroy elements of initialized storage p.
		 */
		void destroy (pointer p) 
		{
			p->~T();
		}

		/**
		 * @brief	Deallocate storage p of deleted elements.
		 */
		void deallocate (pointer p, size_type num) 
		{
			bs_free<Alloc>((void*)p);
		}
	};

   /**
    * @brief	Return that all specializations of this allocator are interchangeable.
    */
   template <class T1, class T2, class Alloc>
   bool operator== (const StdAlloc<T1, Alloc>&,
                    const StdAlloc<T2, Alloc>&) throw() {
       return true;
   }

   /**
    * @brief	Return that all specializations of this allocator are interchangeable.
    */
   template <class T1, class T2, class Alloc>
   bool operator!= (const StdAlloc<T1, Alloc>&,
                    const StdAlloc<T2, Alloc>&) throw() {
       return false;
   }
}

#include "BsMemStack.h"
#include "BsGlobalFrameAlloc.h"
#include "BsMemAllocProfiler.h"