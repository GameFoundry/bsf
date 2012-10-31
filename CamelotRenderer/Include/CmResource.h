#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

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
		Resource();
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