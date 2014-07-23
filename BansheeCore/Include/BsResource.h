#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"
#include "BsCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for all resources.
	 */
	class BS_CORE_EXPORT Resource : public IReflectable, public CoreObject
	{
	public:
		Resource(bool requiresGpuInitialization = true);
		virtual ~Resource() {};

		/**
		 * @brief	Returns the name of the resource.
		 */
		const WString& getName() const;

		/**
		 * @brief	Sets the name of the resource. 
		 */
		void setName(const WString& name);

		/**
		 * @brief	Retrieves meta-data containing various information describing a resource.
		 */
		ResourceMetaDataPtr getMetaData() const { return mMetaData; }

	protected:
		friend class Resources;

		UINT32 mSize;
		ResourceMetaDataPtr mMetaData;

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	public:
		friend class ResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}