#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles all active implementations of IResourceListener interface
	 *			and notifies them when events they're listening to occur.
	 *
	 * @see		IResourceListener
	 */
	class BS_CORE_EXPORT ResourceListenerManager : public Module<ResourceListenerManager>
	{
	public:
		ResourceListenerManager();
		~ResourceListenerManager();

		/**
		 * @brief	Register a new listener to notify for events.
		 */
		void registerListener(IResourceListener* listener);

		/**
		 * @brief	Unregister a listener so it will no longer receive notifications.
		 */
		void unregisterListener(IResourceListener* listener);

		/**
		 * @brief	Marks the listener as dirty which forces the manager to updates
		 *			its internal list of resources for the listener.
		 */
		void markListenerDirty(IResourceListener* listener);

		/**
		 * @brief	Refreshes the resource maps based on dirty listeners and sends out the necessary events.
		 */
		void update();

	private:
		/**
		 * @brief	Triggered by the resources system when a resource has finished loading.
		 */
		void onResourceLoaded(const HResource& resource);

		/**
		 * @brief	Triggered by the resources system just before a resource is unloaded and destroyed.
		 */
		void onResourceDestroyed(const HResource& resource);

		/**
		 * @brief	Sends resource loaded event to all listeners referencing this resource.
		 */
		void sendResourceLoaded(const HResource& resource);

		/**
		 * @brief	Sends resource destroyed event to all listeners referencing this resource.
		 */
		void sendResourceDestroyed(const HResource& resource);

		/**
		 * @brief	Clears all the stored dependencies for the listener.
		 */
		void clearDependencies(IResourceListener* listener);

		/**
		 * @brief	Registers all the resource dependencies for the listener.
		 */
		void addDependencies(IResourceListener* listener);

		HEvent mResourceLoadedConn;
		HEvent mResourceDestroyedConn;

		
		Set<IResourceListener*> mDirtyListeners;
		Map<UINT64, Vector<IResourceListener*>> mResourceToListenerMap;
		Map<IResourceListener*, Vector<UINT64>> mListenerToResourceMap;

		Vector<HResource> mLoadedResources;
		Vector<HResource> mDestroyedResources;

		Vector<HResource> mTempResourceBuffer;

		BS_MUTEX(mMutex)

#if BS_DEBUG_MODE
		Set<IResourceListener*> mActiveListeners;
#endif
	};
}