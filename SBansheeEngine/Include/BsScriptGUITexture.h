#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUITexture : public TScriptGUIElement<ScriptGUITexture>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUITexture")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* texture, 
			GUIImageScaleMode scale, bool transparent, MonoString* style, MonoArray* guiOptions);
		static void internal_setTexture(ScriptGUITexture* nativeInstance, MonoObject* texture);
		static void internal_setTint(ScriptGUITexture* nativeInstance, Color color);

		ScriptGUITexture(MonoObject* instance, GUITexture* texture);
	};
}