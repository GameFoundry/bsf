#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUISliderH : public TScriptGUIElement<ScriptGUISliderH>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUISliderH")

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_setPercent(ScriptGUISliderH* nativeInstance, float percent);
		static float internal_getPercent(ScriptGUISliderH* nativeInstance);
		static void internal_setTint(ScriptGUISliderH* nativeInstance, Color color);

		static void onChanged(MonoObject* instance, float percent);

		ScriptGUISliderH(MonoObject* instance, GUISliderHorz* slider);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, float, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};

	class BS_SCR_BE_EXPORT ScriptGUISliderV : public TScriptGUIElement<ScriptGUISliderV>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUISliderV")

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_setPercent(ScriptGUISliderV* nativeInstance, float percent);
		static float internal_getPercent(ScriptGUISliderV* nativeInstance);
		static void internal_setTint(ScriptGUISliderV* nativeInstance, Color color);

		static void onChanged(MonoObject* instance, float percent);

		ScriptGUISliderV(MonoObject* instance, GUISliderVert* slider);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, float, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}