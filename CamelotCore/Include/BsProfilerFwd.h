#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
	typedef std::basic_string<char, std::char_traits<char>, StdAlloc<char, ProfilerAlloc>> ProfilerString;

	template <typename T, typename A = StdAlloc<T, ProfilerAlloc>>
	using ProfilerVector = std::vector<T, A>;

	template <typename T, typename A = StdAlloc<T, ProfilerAlloc>>
	using ProfilerStack = std::stack<T, std::deque<T, A>>;
}