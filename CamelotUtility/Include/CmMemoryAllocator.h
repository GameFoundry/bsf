#pragma once

#undef min
#undef max

#include <boost/preprocessor.hpp>

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

	/**
	 * @brief	Allocates the specified number of bytes.
	 */
	template<class Alloc> 
	inline void* cm_alloc(UINT32 count)
	{
		return MemoryAllocator<Alloc>::allocate(count);
	}

	/**
	 * @brief	Allocates enough bytes to hold the specified type, but doesn't construct it.
	 */
	template<class T, class Alloc> 
	inline T* cm_alloc()
	{
		return (T*)MemoryAllocator<Alloc>::allocate(sizeof(T));
	}

	/**
	 * @brief	Creates and constructs an array of "count" elements.
	 */
	template<class T, class Alloc> 
	inline T* cm_newN(UINT32 count)
	{
		T* ptr = (T*)MemoryAllocator<Alloc>::allocateArray(sizeof(T), count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&ptr[i]) T;

		return ptr;
	}

/**
 * @brief	Create a new object with the specified allocator and the specified parameters.
 */
#define MAKE_CM_NEW(z, n, unused)                                     \
	template<class Type, class Alloc BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>     \
	Type* cm_new(BOOST_PP_ENUM_BINARY_PARAMS(n, T, &&t) ) { \
		return new (cm_alloc<Alloc>(sizeof(Type))) Type(BOOST_PP_ENUM_PARAMS (n, t));     \
	}

	BOOST_PP_REPEAT_FROM_TO(1, 15, MAKE_CM_NEW, ~)

#undef MAKE_CM_NEW

	// Create a new object with the specified allocator without any parameters
	// (Needs to be separate from parameter version so I don't unnecessarily zero-initialize POD types)
	template<class Type, class Alloc>
	Type* cm_new() 
	{
		return new (cm_alloc<Alloc>(sizeof(Type))) Type;
	}

	/**
	 * @brief	Frees all the bytes allocated at the specified location.
	 */
	template<class Alloc> 
	inline void cm_free(void* ptr)
	{
		MemoryAllocator<Alloc>::free(ptr);
	}

	/**
	 * @brief	Destructs and frees the specified object.
	 */
	template<class Alloc, class T> 
	inline void cm_delete(T* ptr)
	{
		(ptr)->~T();

		MemoryAllocator<Alloc>::free(ptr);
	}

	/**
	 * @brief	Destructs and frees the specified array of objects.
	 */
	template<class Alloc, class T> 
	inline void cm_deleteN(T* ptr, UINT32 count)
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
	inline void* cm_alloc(UINT32 count)
	{
		return MemoryAllocator<GenAlloc>::allocate(count);
	}

	/**
	 * @brief	Allocates enough bytes to hold the specified type, but doesn't construct it.
	 */
	template<class T> 
	inline T* cm_alloc()
	{
		return (T*)MemoryAllocator<GenAlloc>::allocate(sizeof(T));
	}

	/**
	 * @brief	Creates and constructs an array of "count" elements.
	 */
	template<class T> 
	inline T* cm_newN(UINT32 count)
	{
		T* ptr = (T*)MemoryAllocator<GenAlloc>::allocateArray(sizeof(T), count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&ptr[i]) T;

		return ptr;
	}

	// Create a new object with the general allocator and the specified parameters.
#define MAKE_CM_NEW(z, n, unused)                                     \
	template<class Type BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>     \
	Type* cm_new(BOOST_PP_ENUM_BINARY_PARAMS(n, T, &&t) ) { \
	return new (cm_alloc<GenAlloc>(sizeof(Type))) Type(BOOST_PP_ENUM_PARAMS (n, t));     \
	}

	BOOST_PP_REPEAT_FROM_TO(1, 15, MAKE_CM_NEW, ~)

#undef MAKE_CM_NEW

	// Create a new object with the general allocator without any parameters
	// (Needs to be separate from parameter version so I don't unnecessarily zero-initialize POD types)
	template<class Type>
	Type* cm_new() 
	{
		return new (cm_alloc<GenAlloc>(sizeof(Type))) Type;
	}

	/**
	 * @brief	Frees all the bytes allocated at the specified location.
	 */
	inline void cm_free(void* ptr)
	{
		MemoryAllocator<GenAlloc>::free(ptr);
	}

	/**
	 * @brief	Destructs and frees the specified object.
	 */
	template<class T> 
	inline void cm_delete(T* ptr)
	{
		(ptr)->~T();

		MemoryAllocator<GenAlloc>::free(ptr);
	}

	/**
	 * @brief	Destructs and frees the specified array of objects.
	 */
	template<class T> 
	inline void cm_deleteN(T* ptr, UINT32 count)
	{
		for(unsigned int i = 0; i < count; i++)
			ptr[i].~T();

		MemoryAllocator<GenAlloc>::freeArray(ptr, count);
	}
}

namespace CamelotFramework
{
	// Allocator we can use in the standard library
    template <class T, class Alloc = GenAlloc>
	class StdAlloc 
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
			typedef StdAlloc<U, Alloc> other;
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
		StdAlloc() throw() 
		{ }

		StdAlloc(const StdAlloc&) throw() 
		{ }

		template <class U>
		StdAlloc (const StdAlloc<U, Alloc>&) throw() 
		{ }

		~StdAlloc() throw() 
		{ }

		// return maximum number of elements that can be allocated
		size_type max_size () const throw() 
		{
			return std::numeric_limits<std::size_t>::max() / sizeof(T);
		}

		// allocate but don't initialize num elements of type T
		pointer allocate (size_type num, const void* = 0) 
		{
			pointer ret = (pointer)(cm_alloc<Alloc>((UINT32)num*sizeof(T)));
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
			cm_free<Alloc>((void*)p);
		}
	};

   // return that all specializations of this allocator are interchangeable
   template <class T1, class T2, class Alloc>
   bool operator== (const StdAlloc<T1, Alloc>&,
                    const StdAlloc<T2, Alloc>&) throw() {
       return true;
   }
   template <class T1, class T2, class Alloc>
   bool operator!= (const StdAlloc<T1, Alloc>&,
                    const StdAlloc<T2, Alloc>&) throw() {
       return false;
   }
}

#include "CmMemStack.h"