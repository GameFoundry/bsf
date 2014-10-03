#include "BsScriptHandleManager.h"

namespace BansheeEngine
{
	// TODO - Query a list of all IHandle implementations and the types they are activated on
	//  - Make sure the list can be refreshed on assembly reload

	// TODO - Keep a list of every active ScriptSlider* type

	void ScriptHandleManager::refreshHandles()
	{
		// TODO - Query Selection class to detect new and removed items from selection
		//  - Create and destroy IHandle implementations accordingly
		//    - Query new selection for a list of their components and create new handles appropriately

		// TODO - Also handle creation of default move/rotate/scale handles

		// TODO - Call Update() method on all active IHandle implementations

		// TODO - Update matrices of all active sliders from their corresponding SOs
		//   - Need to add a new field to SliderHandle for such matrix
	}

	void ScriptHandleManager::triggerHandles()
	{
		// TODO - See if any sliders are active and if so, trigger their callbacks
	}

	void ScriptHandleManager::queueDrawCommands()
	{
		// TODO - Call Draw methods of all active IHandle implementations
	}
}