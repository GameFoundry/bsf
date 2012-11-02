#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	/**
	 * @brief	Base class for all resources used in the engine.
	 */
	class CM_EXPORT Resource : public IReflectable
	{
	public:
		Resource();
		virtual ~Resource() {};

		void init();
		virtual void initImpl() = 0;

		const String& getUUID() const { return mUUID; }

	protected:
		friend class Resources;
		//virtual void unload() = 0;

		//virtual void calculateSize() = 0;
		//virtual void reload();

		String mUUID; 
		UINT32 mSize;

		// Transient
		bool mInitialized;

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	public:
		friend class ResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}