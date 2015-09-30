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

		/**
		 * @brief	Returns whether or not this resource is allowed to be asynchronously loaded.
		 */
		virtual bool allowAsyncLoading() const { return true; }

	protected:
		friend class Resources;
		friend class ResourceHandleBase;

		/**
		 * @brief	Retrieves a list of all resources that this resource depends on.
		 */
		virtual void getResourceDependencies(FrameVector<HResource>& dependencies) const { }

		/**
		 * @brief	Checks if all the resources this object is dependent on are fully loaded.
		 */
		bool areDependenciesLoaded() const;

		UINT32 mSize;
		ResourceMetaDataPtr mMetaData;

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	public:
		friend class ResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}