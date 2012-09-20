#pragma once

#include "CmPrerequisites.h"
#include "CmUUID.h"

namespace CamelotEngine
{
	/**
	 * @brief	Base class for all resources used in the engine.
	 */
	class CM_EXPORT Resource
	{
	public:
		Resource(/*const UUID& sourceUUID*/) // TODO - Temporarily don't initialize UUID, because I want texture to inherit from resource and UUIDs arent set up yet
			:mSize(0) /*mSourceUUID(sourceUUID),*/
		{}
		virtual ~Resource() {};

	protected:
		UUID mSourceUUID; 
		UINT32 mSize;
	};
}