#include "BsUUID.h"
#include "BsPlatform.h"
#include <chrono>

using namespace std::chrono;

namespace BansheeEngine
{
	String UUIDGenerator::generateRandom()
	{
		return Platform::generateUUID();
	}
};