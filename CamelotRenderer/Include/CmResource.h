#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"
#include "CmUUID.h"

namespace CamelotEngine
{
	enum ResourceState
	{
		RS_Unloaded,
		RS_Loaded
	};

	/**
	 * @brief	Base class for all resources used in the engine.
	 */
	class CM_EXPORT Resource : public IReflectable
	{
	public:
		Resource(/*UUID& _UUID*/) // TODO - Temporarily don't initialize UUID, because I want texture to inherit from resource and UUIDs arent set up yet
			:mSize(0), mLoadState(RS_Unloaded) /*mUUID(_UUID),*/
		{}
		virtual ~Resource() {};

		void load();
		virtual void loadImpl() = 0;

		const String& getUUID() const { return mUUID; }

	protected:
		friend class Resources;
		//virtual void unload() = 0;

		//virtual void calculateSize() = 0;
		//virtual void reload();

		String mUUID; 
		UINT32 mSize;

		// Transient
		ResourceState mLoadState;

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	public:
		friend class ResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}