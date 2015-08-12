#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsInputConfiguration.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for VirtualButton.
	 */
	class BS_SCR_BE_EXPORT ScriptVirtualButton : public ScriptObject <ScriptVirtualButton>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "VirtualButton")

		/**
		 * @brief	Unboxes a boxed managed VirtualButton struct and returns
		 *			the native version of the structure.
		 */
		static VirtualButton unbox(MonoObject* obj);

		/**
		 * @brief	Boxes a native VirtualButton struct and returns
		 *			a managed object containing it.
		 */
		static MonoObject* box(const VirtualButton& value);

	private:
		ScriptVirtualButton(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static UINT32 internal_InitVirtualButton(MonoString* name);
	};
}