#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	std::atomic_uint64_t MemoryCounter::Allocs = 0;
	std::atomic_uint64_t MemoryCounter::Frees = 0;
}