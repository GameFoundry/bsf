#pragma once

#include "CmPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Utility class for generating universally unique identifiers.
	 */
	class CM_UTILITY_EXPORT UUIDGenerator
	{
	public:
		/**
		 * @brief	Generate a new random universally unique identifier.
		 */
		static String generateRandom();
	};
}