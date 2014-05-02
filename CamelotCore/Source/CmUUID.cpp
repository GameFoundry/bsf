#include "CmUUID.h"
#include "CmPlatform.h"
#include <chrono>

using namespace std::chrono;

namespace BansheeEngine
{
	UUIDGenerator::UUIDGenerator()
		:mRandomGenerator((unsigned int)system_clock::now().time_since_epoch().count())
	{
		mHaveMacAddress = Platform::getMACAddress(mMACAddress);
	}

	void appendHex(String& str, UINT8 n)
	{
		static const char* digits = "0123456789abcdef";
		str += digits[(n >> 4) & 0xF];
		str += digits[n & 0xF];
	}


	void appendHex(String& str, UINT16 n)
	{
		appendHex(str, UINT8(n >> 8));
		appendHex(str, UINT8(n & 0xFF));
	}


	void appendHex(String& str, UINT32 n)
	{
		appendHex(str, UINT16(n >> 16));
		appendHex(str, UINT16(n & 0xFFFF));
	}


	String UUIDGenerator::generateRandom()
	{
		ScopedSpinLock lock;

		auto timestamp = system_clock::now().time_since_epoch().count();

		UINT32 timeLow = UINT32(timestamp & 0xFFFFFFFF);
		UINT16 timeMid = UINT16((timestamp >> 32) & 0xFFFF);
		UINT16 timeHiAndVersion = UINT16((timestamp >> 48) & 0x0FFF) + (UUIDV_TimeBased << 12);
		UINT16 clockSeq = (UINT16(mRandomGenerator() >> 4) & 0x3FFF) | 0x8000;

		String result;
		result.reserve(36);

		appendHex(result, timeLow);
		result += '-';

		appendHex(result, timeMid);
		result += '-';

		appendHex(result, timeHiAndVersion);
		result += '-';

		appendHex(result, clockSeq);
		result += '-';

		if (mHaveMacAddress)
		{
			for (int i = 0; i < sizeof(MACAddress); ++i)
				appendHex(result, mMACAddress.value[i]);
		}
		else
		{
			for (int i = 0; i < sizeof(MACAddress); ++i)
				appendHex(result, (UINT8)(mRandomGenerator() % 255));
		}

		return result;
	}
};