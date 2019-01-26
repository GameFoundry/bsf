//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/GUI/BsScriptGUIElement.h"

namespace bs
{
	struct __GUIContentInterop;

	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for GUILabel. */
	class BS_SCR_BE_EXPORT ScriptGUILabel : public TScriptGUIElement<ScriptGUILabel>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUILabel")

	private:
		ScriptGUILabel(MonoObject* instance, GUILabel* label);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, __GUIContentInterop* content, MonoString* style, 
			MonoArray* guiOptions);
		static void internal_setContent(ScriptGUILabel* nativeInstance, __GUIContentInterop* content);
		static void internal_setTint(ScriptGUILabel* nativeInstance, Color* color);
	};

	/** @} */
}