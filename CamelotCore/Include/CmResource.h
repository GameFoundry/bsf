#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"
#include "CmCoreObject.h"

namespace CamelotFramework
{
	/**
	 * @brief	Base class for all resources used in the engine.
	 */
	class CM_EXPORT Resource : public IReflectable, public CoreObject
	{
	public:
		Resource(bool requiresGpuInitialization = true);
		virtual ~Resource() {};

		const String& getUUID() const { return mUUID; }

		/**
		 * @brief	Creates a new resource handle. Should only be called by internal methods.
		 */
		static HResource _createResourceHandle(ResourcePtr obj);

	protected:
		friend class Resources;

		String mUUID; 
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