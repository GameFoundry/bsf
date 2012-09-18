#pragma once

#include "CmPrerequisites.h"
#include "CmUUID.h"

namespace CamelotEngine
{
	/**
	 * @brief	Base class for all resources used in the engine.
	 */
	class Resource
	{
	public:
		Resource(const UUID& sourceUUID):mSourceUUID(sourceUUID) {}
		virtual ~Resource() {};

	private:
		UUID mSourceUUID; 
	};
}