#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	// TODO Low priority - Add debug option that would remember a call stack for each resource initialization,
	// so when we fail to release one we know which one it is.
	
	/**
	 * @brief	Manager that keeps track of all active CoreObjects.
	 * 			
	 * @note	Thread safe.
	 */
	class CM_EXPORT CoreObjectManager : public Module<CoreObjectManager>
	{
	public:
		CoreObjectManager();
		~CoreObjectManager();

		/**
		 * @brief	Registers a new CoreObject notifying the manager the object
		 * 			is created.
		 */
		UINT64 registerObject(CoreObject* object);

		/**
		 * @brief	Unregisters a CoreObject notifying the manager the object
		 * 			is destroyed.
		 */
		void unregisterObject(CoreObject* object);

	private:
		UINT64 mNextAvailableID;
		Map<UINT64, CoreObject*>::type mObjects;
		CM_MUTEX(mObjectsMutex);
	};
}