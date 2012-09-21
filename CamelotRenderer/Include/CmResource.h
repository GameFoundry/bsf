#pragma once

#include "CmPrerequisites.h"
#include "CmISerializable.h"
#include "CmUUID.h"

namespace CamelotEngine
{
	/**
	 * @brief	Base class for all resources used in the engine.
	 */
	class CM_EXPORT Resource : public ISerializable
	{
	public:
		virtual ~Resource() {};

		//public 

	protected:
		friend class Resources;

		Resource(/*const UUID& sourceUUID*/) // TODO - Temporarily don't initialize UUID, because I want texture to inherit from resource and UUIDs arent set up yet
			:mSize(0) /*mSourceUUID(sourceUUID),*/
		{}

		//virtual void load() = 0;
		//virtual void unload() = 0;

		//virtual void calculateSize() = 0;
		//virtual void reload();

		UUID mSourceUUID; 
		UINT32 mSize;

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	public:
		friend class ResourceST;

		virtual SerializableType* getSerializable() const;

		static Resource* newObject()
		{
			return new Resource();
		}
	};
}