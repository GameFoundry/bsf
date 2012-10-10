#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"
#include "CmUUID.h"

namespace CamelotEngine
{
	/**
	 * @brief	Base class for all resources used in the engine.
	 */
	class CM_EXPORT Resource : public IReflectable
	{
	public:
		Resource(/*const UUID& sourceUUID*/) // TODO - Temporarily don't initialize UUID, because I want texture to inherit from resource and UUIDs arent set up yet
			:mSize(0) /*mSourceUUID(sourceUUID),*/
		{}
		virtual ~Resource() {};

		virtual void load() = 0;

	protected:
		friend class Resources;
		//virtual void unload() = 0;

		//virtual void calculateSize() = 0;
		//virtual void reload();

		UUID mSourceUUID; 
		UINT32 mSize;

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	public:
		friend class ResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}