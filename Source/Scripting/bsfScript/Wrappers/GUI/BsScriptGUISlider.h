//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/GUI/BsScriptGUIElement.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for GUISliderH.  */
	class BS_SCR_BE_EXPORT ScriptGUISliderH : public TScriptGUIElement<ScriptGUISliderH>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUISliderH")

	private:
		ScriptGUISliderH(MonoObject* instance, GUISliderHorz* slider);

		/**	Triggered when the native slider is moved. */
		void onChanged(float percent);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_setPercent(ScriptGUISliderH* nativeInstance, float percent);
		static float internal_getPercent(ScriptGUISliderH* nativeInstance);
		static float internal_getValue(ScriptGUISliderH* nativeInstance);
		static void internal_setValue(ScriptGUISliderH* nativeInstance, float percent);
		static void internal_setRange(ScriptGUISliderH* nativeInstance, float min, float max);
		static float internal_getRangeMaximum(ScriptGUISliderH* nativeInstance);
		static float internal_getRangeMinimum(ScriptGUISliderH* nativeInstance);
		static void internal_setStep(ScriptGUISliderH* nativeInstance, float step);
		static float internal_getStep(ScriptGUISliderH* nativeInstance);
		static void internal_setTint(ScriptGUISliderH* nativeInstance, Color* color);

		typedef void(BS_THUNKCALL *OnChangedThunkDef) (MonoObject*, float, MonoException**);
		static OnChangedThunkDef onChangedThunk;
	};

	/**	Interop class between C++ & CLR for GUISliderV. */
	class BS_SCR_BE_EXPORT ScriptGUISliderV : public TScriptGUIElement<ScriptGUISliderV>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUISliderV")

	private:
		ScriptGUISliderV(MonoObject* instance, GUISliderVert* slider);

		/**	Triggered when the native slider is moved. */
		void onChanged(float percent);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_setPercent(ScriptGUISliderV* nativeInstance, float percent);
		static float internal_getPercent(ScriptGUISliderV* nativeInstance);
		static float internal_getValue(ScriptGUISliderV* nativeInstance);
		static void internal_setValue(ScriptGUISliderV* nativeInstance, float percent);
		static void internal_setRange(ScriptGUISliderV* nativeInstance, float min, float max);
		static float internal_getRangeMaximum(ScriptGUISliderV* nativeInstance);
		static float internal_getRangeMinimum(ScriptGUISliderV* nativeInstance);
		static void internal_setStep(ScriptGUISliderV* nativeInstance, float step);
		static float internal_getStep(ScriptGUISliderV* nativeInstance);
		static void internal_setTint(ScriptGUISliderV* nativeInstance, Color* color);

		typedef void(BS_THUNKCALL *OnChangedThunkDef) (MonoObject*, float, MonoException**);
		static OnChangedThunkDef onChangedThunk;
	};

	/** @} */
}
