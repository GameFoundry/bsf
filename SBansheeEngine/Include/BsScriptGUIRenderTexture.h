#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIRenderTexture. 
	 */
	class BS_SCR_BE_EXPORT ScriptGUIRenderTexture : public TScriptGUIElement < ScriptGUIRenderTexture >
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIRenderTexture")

	private:
		ScriptGUIRenderTexture(MonoObject* instance, GUIRenderTexture* texture);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, ScriptRenderTexture2D* texture,
			MonoString* style, MonoArray* guiOptions);
		static void internal_setTexture(ScriptGUIRenderTexture* nativeInstance, ScriptRenderTexture2D* texture);
		static void internal_setTint(ScriptGUIRenderTexture* nativeInstance, Color color);
	};
}