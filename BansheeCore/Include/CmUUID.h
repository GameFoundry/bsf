#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "BsSpinLock.h"
#include <random>

namespace BansheeEngine
{
	/**
	 * @brief	Utility class for generating universally unique identifiers.
	 *
	 * @note	Thread safe.
	 */
	class BS_CORE_EXPORT UUIDGenerator : public Module<UUIDGenerator>
	{
		enum UUIDVersion
		{
			UUIDV_TimeBased = 0x01
		};

	public:
		UUIDGenerator();

		/**
		 * @brief	Generate a new random universally unique identifier.
		 */
		String generateRandom();

	private:
		std::mt19937 mRandomGenerator;
		MACAddress mMACAddress;
		SpinLock mSpinLock;
		bool mHaveMacAddress;
	};
}