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

		const String& getName() const { return mName; }
		void setName(const String& name) { mName = name; }

	protected:
		friend class Resources;

		UINT32 mSize;
		String mName;

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	public:
		friend class ResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}