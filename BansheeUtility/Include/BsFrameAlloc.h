#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Frame allocator. Performs very fast allocations but can only free all of its memory at once.
	 * 			Perfect for allocations that last just a single frame.
	 * 			
	 * @note	Not thread safe with an exception. "alloc" and "clear" methods need to be called from the same thread.
	 * 			"dealloc" is thread safe and can be called from any thread.
	 */
	class BS_UTILITY_EXPORT FrameAlloc
	{
	private:
		class MemBlock
		{
		public:
			MemBlock(UINT32 size);
			~MemBlock();

			UINT8* alloc(UINT8 amount);
			void clear();

			UINT8* mData;
			UINT32 mFreePtr;
			UINT32 mSize;
		};

	public:
		FrameAlloc(UINT32 blockSize = 1024 * 1024);
		~FrameAlloc();

		/**
		 * @brief	Allocates a new block of memory of the specified size.
		 *
		 * @param	amount	Amount of memory to allocate, in bytes.
		 * 					
		 * @note	Not thread safe.
		 */
		UINT8* alloc(UINT32 amount);

		/**
		 * @brief	Deallocates a previously allocated block of memory.
		 *
		 * @note	No deallocation is actually done here. This method is only used for debug purposes
		 * 			so it is easier to track down memory leaks and corruption.
		 * 			
		 *			Thread safe.
		 */
		void dealloc(UINT8* data);

		/**
		 * @brief	Deallocates all allocated memory.
		 * 			
		 * @note	Not thread safe.
		 */
		void clear();

	private:
		UINT32 mBlockSize;
		Vector<MemBlock*> mBlocks;
		MemBlock* mFreeBlock;
		std::atomic<UINT32> mTotalAllocBytes;

#if BS_DEBUG_MODE
		UINT32 mAllocId;
		Set<UINT32> mActiveAllocs;
#endif

		MemBlock* allocBlock(UINT32 wantedSize);
		void deallocBlock(MemBlock* block);
	};

	/**
	 * @brief	Allocator for the standard library that internally uses a
	 * 			frame allocator.
	 */
	template <class T>
	class StdFrameAlloc
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
			typedef StdFrameAlloc<U> other;
		};

		StdFrameAlloc() throw()
			:mFrameAlloc(nullptr)
		{ }

		StdFrameAlloc(FrameAlloc* frameAlloc) throw()
			:mFrameAlloc(frameAlloc)
		{ }

		StdFrameAlloc(const StdFrameAlloc& alloc) throw()
			:mFrameAlloc(alloc.mFrameAlloc)
		{ }

		template <class U>
		StdFrameAlloc(const StdFrameAlloc<U>& alloc) throw()
			:mFrameAlloc(alloc.mFrameAlloc)
		{ }

		~StdFrameAlloc() throw()
		{ }

		/**
		 * @brief	Return address of value.
		 */
		pointer address(reference value) const
		{
			return &value;
		}

		/**
		 * @brief	Return address of value.
		 */
		const_pointer address(const_reference value) const
		{
			return &value;
		}

		/**
		 * @brief	Return maximum number of elements that can be allocated.
		 */
		size_type max_size() const throw()
		{
			return std::numeric_limits<std::size_t>::max() / sizeof(T);
		}

		/**
		 * @brief	Allocate but don't initialize number elements of type T.
		 */
		pointer allocate(size_type num, const void* = 0)
		{
			pointer ret = (pointer)(mFrameAlloc->alloc((UINT32)num*sizeof(T)));
			return ret;
		}

		/**
		 * @brief	Initialize elements of allocated storage p with value "value".
		 */
		void construct(pointer p, const T& value)
		{
			new((void*)p)T(value);
		}

		/**
		 * @brief	Destroy elements of initialized storage p.
		 */
		void destroy(pointer p)
		{
			p->~T();
		}

		/**
		 * @brief	Deallocate storage p of deleted elements.
		 */
		void deallocate(pointer p, size_type num)
		{
			mFrameAlloc->dealloc((UINT8*)p);
		}

		FrameAlloc* mFrameAlloc;
	};

	/**
	 * @brief	Return that all specializations of this allocator are interchangeable.
	 */
	template <class T1, class T2>
	bool operator== (const StdFrameAlloc<T1>&,
		const StdFrameAlloc<T2>&) throw() {
		return true;
	}

	/**
	 * @brief	Return that all specializations of this allocator are interchangeable.
	 */
	template <class T1, class T2>
	bool operator!= (const StdFrameAlloc<T1>&,
		const StdFrameAlloc<T2>&) throw() {
		return false;
	}
}