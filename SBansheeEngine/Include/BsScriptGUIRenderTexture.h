#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIRenderTexture : public TScriptGUIElement < ScriptGUIRenderTexture >
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIRenderTexture")

	private:
		static void internal_createInstance(MonoObject* instance, ScriptRenderTexture2D* texture,
			MonoString* style, MonoArray* guiOptions);
		static void internal_setTexture(ScriptGUIRenderTexture* nativeInstance, ScriptRenderTexture2D* texture);

		ScriptGUIRenderTexture(MonoObject* instance, GUIRenderTexture* texture);
	};
}