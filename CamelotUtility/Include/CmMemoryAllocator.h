#pragma once

#undef min
#undef max

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
		static inline void* allocate(UINT32 bytes)
		{
			return malloc(bytes);
		}

		static inline void* allocateArray(UINT32 bytes, UINT32 count)
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
	inline T* __cm_construct_array(UINT32 count)
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
	inline void __cm_destruct_array(T* ptr, UINT32 count)
	{
		// This might seem a bit weird if T is a built-in type or a pointer, but
		// standard allows us to call destructor on such types (they don't do anything)
		for(unsigned int i = 0; i < count; i++)
			ptr[i].~T();

		MemoryAllocator<category>::freeArray(ptr, count);
	}

	/**
	 * @brief	General allocator provided by the OS. Use for persistent long term allocations,
	 * 			and allocations that don't happen often.
	 */
	class GenAlloc
	{ };

	/**
	 * @brief	Allocator used for allocating small amounts of temporary memory that
	 * 			used and then quickly released
	 */
	class ScratchAlloc
	{ };

	/**
	 * @brief	Pool allocator that is only suited for allocating one specific type of data. Most useful when you are
	 * 			often allocating one certain data type, with no specific allocation or deallocation order.
	 */
	class PoolAlloc
	{ };
}

#define CM_NEW(T, category) new (CamelotFramework::MemoryAllocator<category>::allocate(sizeof(T)))
#define CM_NEW_BYTES(count, category) (UINT8*)CamelotFramework::MemoryAllocator<category>::allocate(count)
#define CM_NEW_ARRAY(T, count, category) CamelotFramework::__cm_construct_array<T, category>(count)
#define CM_DELETE(ptr, T, category) {(ptr)->~T(); CamelotFramework::MemoryAllocator<category>::free(ptr);}
#define CM_DELETE_BYTES(ptr, category) CamelotFramework::MemoryAllocator<category>::free(ptr)
#define CM_DELETE_ARRAY(ptr, T, count, category) CamelotFramework::__cm_destruct_array<T, category>(ptr, count)

namespace CamelotFramework
{
	// Allocators we can use in the standard library
	
	template <class T>
	class StdGenAlloc 
	{
		public:
		// type definitions
		typedef T        value_type;
		typedef T*       pointer;
		typedef const T* const_pointer;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef std::size_t    size_type;
		typedef std::ptrdiff_t difference_type;

		// rebind allocator to type U
		template <class U>
		struct rebind 
		{
			typedef StdGenAlloc<U> other;
		};

		// return address of values
		pointer address (reference value) const 
		{
			return &value;
		}
		const_pointer address (const_reference value) const 
		{
			return &value;
		}

		/* constructors and destructor
		* - nothing to do because the allocator has no state
		*/
		StdGenAlloc() throw() 
		{ }

		StdGenAlloc(const StdGenAlloc&) throw() 
		{ }

		template <class U>
		StdGenAlloc (const StdGenAlloc<U>&) throw() 
		{ }

		~StdGenAlloc() throw() 
		{ }

		// return maximum number of elements that can be allocated
		size_type max_size () const throw() 
		{
			return std::numeric_limits<std::size_t>::max() / sizeof(T);
		}

		// allocate but don't initialize num elements of type T
		pointer allocate (size_type num, const void* = 0) 
		{
			pointer ret = (pointer)(CM_NEW_BYTES((UINT32)num*sizeof(T), GenAlloc));
			return ret;
		}

		// initialize elements of allocated storage p with value value
		void construct (pointer p, const T& value) 
		{
			// initialize memory with placement new
			new((void*)p)T(value);
		}

		// destroy elements of initialized storage p
		void destroy (pointer p) 
		{
			// destroy objects by calling their destructor
			p->~T();
		}

		// deallocate storage p of deleted elements
		void deallocate (pointer p, size_type num) 
		{
			// print message and deallocate memory with global delete
			CM_DELETE_BYTES((void*)p, GenAlloc);
		}
	};

   // return that all specializations of this allocator are interchangeable
   template <class T1, class T2>
   bool operator== (const StdGenAlloc<T1>&,
                    const StdGenAlloc<T2>&) throw() {
       return true;
   }
   template <class T1, class T2>
   bool operator!= (const StdGenAlloc<T1>&,
                    const StdGenAlloc<T2>&) throw() {
       return false;
   }
}

#include "CmMemStack.h"