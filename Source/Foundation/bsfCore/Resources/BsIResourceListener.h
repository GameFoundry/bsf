//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"

namespace bs
{
	/** @addtogroup Implementation
	 *  @{
	 */

	/** Interface that allows the implementing class to be notified when the resources it is referencing change. */
	class BS_CORE_EXPORT IResourceListener
	{
	public:
		IResourceListener();
		virtual ~IResourceListener();

	protected:
		friend class ResourceListenerManager;

		/**
		 * Retrieves all the resources that the class depends on.
		 *
		 * @note	Derived implementations must add the resources to the provided @p resources array.
		 */
		virtual void getListenerResources(Vector<HResource>& resources) = 0;

		/**	Marks the resource dependencies list as dirty and schedules it for rebuild. */
		virtual void markListenerResourcesDirty();

		/**	Called when a resource has been fully loaded. */
		virtual void notifyResourceLoaded(const HResource& resource) { }

		/**	Called when the internal resource the resource handle is pointing to changes. */
		virtual void notifyResourceChanged(const HResource& resource) { }
	};

	/** @} */
}
