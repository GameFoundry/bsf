#pragma once

namespace CamelotFramework
{
	/**
	 * @brief	Specialized allocator for profiler so we can avoid tracking internal profiler memory allocations
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
}