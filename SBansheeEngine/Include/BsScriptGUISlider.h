#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUISliderH. 
	 */
	class BS_SCR_BE_EXPORT ScriptGUISliderH : public TScriptGUIElement<ScriptGUISliderH>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUISliderH")

	private:
		ScriptGUISliderH(MonoObject* instance, GUISliderHorz* slider);

		/**
		 * @brief	Triggered when the native slider is moved.
		 */
		static void onChanged(MonoObject* instance, float percent);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_setPercent(ScriptGUISliderH* nativeInstance, float percent);
		static float internal_getPercent(ScriptGUISliderH* nativeInstance);
		static float internal_getValue(ScriptGUISliderH* nativeInstance);
		static void internal_setValue(ScriptGUISliderH* nativeInstance, float percent);
		static void internal_setRange(ScriptGUISliderH* nativeInstance, float min, float max);
		static void internal_setStep(ScriptGUISliderH* nativeInstance, float step);
		static void internal_setTint(ScriptGUISliderH* nativeInstance, Color color);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, float, MonoException**);
		static OnChangedThunkDef onChangedThunk;
	};

	/**
	 * @brief	Interop class between C++ & CLR for GUISliderV. 
	 */
	class BS_SCR_BE_EXPORT ScriptGUISliderV : public TScriptGUIElement<ScriptGUISliderV>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUISliderV")

	private:
		ScriptGUISliderV(MonoObject* instance, GUISliderVert* slider);

		/**
		 * @brief	Triggered when the native slider is moved.
		 */
		static void onChanged(MonoObject* instance, float percent);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_setPercent(ScriptGUISliderV* nativeInstance, float percent);
		static float internal_getPercent(ScriptGUISliderV* nativeInstance);
		static float internal_getValue(ScriptGUISliderV* nativeInstance);
		static void internal_setValue(ScriptGUISliderV* nativeInstance, float percent);
		static void internal_setRange(ScriptGUISliderV* nativeInstance, float min, float max);
		static void internal_setStep(ScriptGUISliderV* nativeInstance, float step);
		static void internal_setTint(ScriptGUISliderV* nativeInstance, Color color);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, float, MonoException**);
		static OnChangedThunkDef onChangedThunk;
	};
}