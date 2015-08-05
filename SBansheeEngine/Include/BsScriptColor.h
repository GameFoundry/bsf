#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsColor.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ScriptColor.
	 */
	class BS_SCR_BE_EXPORT ScriptColor : public ScriptObject <ScriptColor>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Color")

		/**
		 * @brief	Unboxes a boxed managed Color struct and returns
		 *			the native version of the structure.
		 */
		static Color unbox(MonoObject* obj);

		/**
		 * @brief	Boxes a native Color struct and returns
		 *			a managed object containing it.
		 */
		static MonoObject* box(const Color& value);

	private:
		ScriptColor(MonoObject* instance);
	};
}