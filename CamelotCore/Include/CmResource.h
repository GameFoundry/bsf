#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"
#include "CmCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for all resources.
	 */
	class CM_EXPORT Resource : public IReflectable, public CoreObject
	{
	public:
		Resource(bool requiresGpuInitialization = true);
		virtual ~Resource() {};

		/**
		 * @brief	Returns the name of the resource.
		 */
		const String& getName() const { return mName; }

		/**
		 * @brief	Sets the name of the resource. 
		 */
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