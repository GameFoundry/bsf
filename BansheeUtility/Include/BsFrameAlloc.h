#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Frame allocator. Performs very fast allocations but can only free all of its memory at once.
	 * 			Perfect for allocations that last just a single frame.
	 * 			
	 * @note	Not thread safe with an exception. ::alloc and ::clear methods need to be called from the same thread.
	 * 			::dealloc is thread safe and can be called from any thread.
	 */
	class BS_UTILITY_EXPORT FrameAlloc
	{
	private:
		/**
		 * @brief	A single block of memory within a frame allocator.
		 */
		class MemBlock
		{
		public:
			MemBlock(UINT32 size);
			~MemBlock();

			/**
			 * @brief	Allocates a piece of memory within the block. Caller must ensure
			 *			the block has enough empty space.
			 */
			UINT8* alloc(UINT32 amount);

			/**
			 * @brief	Releases all allocations within a block but doesn't actually free the memory.
			 */
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
		 * @brief	Allocates and constructs a new object.
		 *	
		 * @note	Not thread safe.
		 */
		template<class T, class... Args>
		T* alloc(Args &&...args)
		{
			return new ((T*)alloc(sizeof(T))) T(std::forward<Args>(args)...);
		}

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
		 * @brief	Deallocates and destructs a previously allocated object.
		 *
		 * @note	No deallocation is actually done here. This method is only used to call the destructor
		 *			and for debug purposes so it is easier to track down memory leaks and corruption.
		 * 			
		 *			Thread safe.
		 */
		template<class T>
		void dealloc(T* obj)
		{
			if (obj != nullptr)
				obj->~T();

			dealloc((UINT8*)obj);
		}

		/**
		 * @brief	Starts a new frame. Next call to ::clear will only clear memory
		 *			allocated past this point.
		 */
		void markFrame();

		/**
		 * @brief	Deallocates all allocated memory since the last call to ::markFrame
		 *			(or all the memory if there was no call to ::markFrame).
		 * 			
		 * @note	Not thread safe.
		 */
		void clear();

		/**
		 * @brief	Changes the frame allocator owner thread. After the owner
		 *			thread has changed only allocations from that thread can be made.
		 */
		void setOwnerThread(BS_THREAD_ID_TYPE thread);

	private:
		UINT32 mBlockSize;
		Vector<MemBlock*> mBlocks;
		MemBlock* mFreeBlock;
		UINT32 mNextBlockIdx;
		std::atomic<UINT32> mTotalAllocBytes;
		void* mLastFrame;

#if BS_DEBUG_MODE
		BS_THREAD_ID_TYPE mOwnerThread;
#endif

		/**
		 * @brief	Allocates a dynamic block of memory of the wanted size. The exact allocation size
		 *			might be slightly higher in order to store block meta data.
		 */
		MemBlock* allocBlock(UINT32 wantedSize);

		/**
		 * @brief	Frees a memory block.
		 */
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
		typedef T value_type;

		StdFrameAlloc() noexcept 
			:mFrameAlloc(nullptr)
		{ }

		StdFrameAlloc(FrameAlloc* alloc) noexcept
			:mFrameAlloc(alloc)
		{ }

		template<class T> StdFrameAlloc(const StdFrameAlloc<T>& alloc) noexcept
			:mFrameAlloc(alloc.mFrameAlloc)
		{ }

		template<class T> bool operator==(const StdFrameAlloc<T>&) const noexcept { return true; }
		template<class T> bool operator!=(const StdFrameAlloc<T>&) const noexcept { return false; }

		/**
		 * @brief	Allocate but don't initialize number elements of type T.
		 */
		T* allocate(const size_t num) const
		{
			if (num == 0)
				return nullptr;

			if (num > static_cast<size_t>(-1) / sizeof(T))
				throw std::bad_array_new_length();

			void* const pv = mFrameAlloc->alloc((UINT32)(num * sizeof(T)));
			if (!pv)
				throw std::bad_alloc();

			return static_cast<T*>(pv);
		}

		/**
		 * @brief	Deallocate storage p of deleted elements.
		 */
		void deallocate(T* p, size_t num) const noexcept
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