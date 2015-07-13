#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptObjectManager : public Module <ScriptObjectManager>
	{
	public:
		ScriptObjectManager();

		void registerScriptObject(ScriptObjectBase* instance);
		void unregisterScriptObject(ScriptObjectBase* instance);

		void refreshAssemblies();

		void update();

		/**
		 * @brief	Call this when object finalizer is triggered. At the end of the frame
		 *			all objects queued with this method will have their _onManagedInstanceDeleted methods
		 *			triggered.
		 *
		 * @note	Thread safe.
		 */
		void notifyObjectFinalized(ScriptObjectBase* instance);

		/**
		 * @brief	Triggers _onManagedInstanceDeleted deleted callbacks on all objects that were finalized this frame.
		 *			This allows the native portions of those objects to properly clean up any resources.
		 *
		 * @note	Sim thread.
		 */
		void processFinalizedObjects();

		/**
		 * @brief	Triggered right after a domain was reloaded. This signals the outside world that they should
		 *			update any kept Mono references as the old ones will no longer be valid.
		 */
		Event<void()> onRefreshDomainLoaded;

		Event<void()> onRefreshStarted;
		Event<void()> onRefreshComplete;
	private:
		Set<ScriptObjectBase*> mScriptObjects;

		Vector<ScriptObjectBase*> mFinalizedObjects[2];
		std::atomic<UINT32> mFinalizedQueueIdx;
	};
}