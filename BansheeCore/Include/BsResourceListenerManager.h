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

		/**
		 * @brief	Forces the listener to send out events about the specified resource immediately, instead
		 *			of waiting for the next "update()" call.
		 */
		void notifyListeners(const String& resourceUUID);

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
		 * @brief	Triggered by the resources system after a resource handle is modified (i.e. points to a new resource).
		 */
		void onResourceModified(const HResource& resource);

		/**
		 * @brief	Sends resource loaded event to all listeners referencing this resource.
		 */
		void sendResourceLoaded(const HResource& resource);

		/**
		 * @brief	Sends resource destroyed event to all listeners referencing this resource.
		 */
		void sendResourceDestroyed(const HResource& resource);

		/**
		 * @brief	Sends resource modified event to all listeners referencing this resource.
		 */
		void sendResourceModified(const HResource& resource);

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
		HEvent mResourceModifiedConn;
		
		Set<IResourceListener*> mDirtyListeners;
		Map<UINT64, Vector<IResourceListener*>> mResourceToListenerMap;
		Map<IResourceListener*, Vector<UINT64>> mListenerToResourceMap;

		Map<String, HResource> mLoadedResources;
		Map<String, HResource> mDestroyedResources;
		Map<String, HResource> mModifiedResources;

		Vector<HResource> mTempResourceBuffer;

		BS_RECURSIVE_MUTEX(mMutex);

#if BS_DEBUG_MODE
		Set<IResourceListener*> mActiveListeners;
#endif
	};
}