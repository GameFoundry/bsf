#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/** @addtogroup Utility-Core
	 *  @{
	 */

	/**
	 * Utility class for generating universally unique identifiers.
	 *
	 * @note	Thread safe.
	 */
	class BS_CORE_EXPORT UUIDGenerator
	{
	public:
		/**	Generate a new random universally unique identifier. */
		static String generateRandom();
	};

	/** @} */
}