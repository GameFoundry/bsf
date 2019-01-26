//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "Math/BsPlane.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for Plane. */
	class BS_SCR_BE_EXPORT ScriptPlane : public ScriptObject <ScriptPlane>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Plane")

		/** Unboxes a boxed managed Plane struct and returns the native version of the structure. */
		static Plane unbox(MonoObject* obj);

		/**	Boxes a native Plane struct and returns a managed object containing it. */
		static MonoObject* box(const Plane& value);

	private:
		ScriptPlane(MonoObject* instance);
	};

	/** @} */
}