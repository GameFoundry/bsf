#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interface that allows the implementing class to be notified
	 *			when the resources it is referencing change.
	 */
	class BS_CORE_EXPORT IResourceListener
	{
	public:
		IResourceListener();
		virtual ~IResourceListener();

	protected:
		friend class ResourceListenerManager;

		/**
		 * @brief	Retrieves all the resources that the class depends on.
		 *
		 * @note	You must add the resources to the provided "resources" array.
		 */
		virtual void getResourceDependencies(Vector<HResource>& resources) = 0;

		/**
		 * @brief	Marks the resource dependencies list as dirty and schedules it for rebuild.
		 */
		virtual void markResourcesDirty();

		/**
		 * @brief	Called when a resource has been fully loaded.
		 */
		virtual void notifyResourceLoaded(const HResource& resource) { }

		/**
		 * @brief	Called when the internal resource the resource handle 
		 *			is pointing to changes.
		 */
		virtual void notifyResourceChanged(const HResource& resource) { }

		/**
		 * @brief	Called just before the resource handle is destroyed and resource
		 *			unloaded.
		 */
		virtual void notifyResourceDestroyed(const HResource& resource) { }
	};
}