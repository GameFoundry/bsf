#pragma once

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup Memory
	 *  @{
	 */

	/**
	 * Specialized allocator for profiler so we can avoid tracking internal profiler memory allocations which would skew 
	 * profiler results.
	 */
	class ProfilerAlloc
	{};

	/** Memory allocator providing a generic implementation. Specialize for specific categories as needed. */
	template<>
	class MemoryAllocator<ProfilerAlloc> : public MemoryAllocatorBase
	{
	public:
		/** Allocates the given number of bytes. */
		static void* allocate(size_t bytes)
		{
			return malloc(bytes);
		}

		/** Allocates the given a number of objects, each of the given number of bytes. */
		static void* allocateArray(size_t bytes, UINT32 count)
		{
			return malloc(bytes * count);
		}

		/** Frees memory previously allocated with allocate(). */
		static void free(void* ptr)
		{
			::free(ptr);
		}

		/**
		 * Frees memory previously allocated with freeArray(). @p count must match the original value when array was allocated.
		 */
		static void freeArray(void* ptr, UINT32 count)
		{
			::free(ptr);
		}
	};

	/** @} */
	/** @endcond */
}