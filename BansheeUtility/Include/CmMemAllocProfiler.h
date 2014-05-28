#pragma once

namespace BansheeEngine
{
	/**
	 * @brief	Specialized allocator for profiler so we can avoid tracking internal profiler memory allocations
	 * 			which would skew profiler results.
	 */
	class ProfilerAlloc
	{};

	/**
	 * @brief	Memory allocator providing a generic implementation. 
	 * 			Specialize for specific categories as needed.
	 */
	template<>
	class MemoryAllocator<ProfilerAlloc> : public MemoryAllocatorBase
	{
	public:
		/**
		 * @brief	Allocates the given number of bytes.
		 */
		static inline void* allocate(size_t bytes)
		{
			return malloc(bytes);
		}

		/**
		 * @brief	Allocates the given a number of objects, each of the given number of bytes.
		 */
		static inline void* allocateArray(size_t bytes, UINT32 count)
		{
			return malloc(bytes * count);
		}

		/**
		 * @brief	Frees memory previously allocated with "allocate".
		 */
		static inline void free(void* ptr)
		{
			::free(ptr);
		}

		/**
		 * @brief	Frees memory previously allocated with "freeArray". "count" must match the
		 * 			original value when array was allocated.
		 */
		static inline void freeArray(void* ptr, UINT32 count)
		{
			::free(ptr);
		}
	};
}