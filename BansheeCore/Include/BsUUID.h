//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
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
		/**
		 * @brief	Type of UUID generation to use.
		 */
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