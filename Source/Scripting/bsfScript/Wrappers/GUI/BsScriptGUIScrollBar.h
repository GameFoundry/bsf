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

	class GUIScrollBar;
	
	/**	Interop class between C++ & CLR for GUIScrollBar.  */
	class BS_SCR_BE_EXPORT ScriptGUIScrollBar : public TScriptGUIElement<ScriptGUIScrollBar>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUIScrollBar")

	private:
		ScriptGUIScrollBar(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static float internal_GetPosition(ScriptGUIElementBaseTBase* nativeInstance);
		static void internal_SetPosition(ScriptGUIElementBaseTBase* nativeInstance, float percent);
		static float internal_GetHandleSize(ScriptGUIElementBaseTBase* nativeInstance);
		static void internal_SetHandleSize(ScriptGUIElementBaseTBase* nativeInstance, float percent);
		static void internal_SetTint(ScriptGUIElementBaseTBase* nativeInstance, Color* color);
	};

	/**	Interop class between C++ & CLR for GUIScrollBarH.  */
	class BS_SCR_BE_EXPORT ScriptGUIScrollBarH : public TScriptGUIElement<ScriptGUIScrollBarH>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUIScrollBarH")

	private:
		ScriptGUIScrollBarH(MonoObject* instance, GUIScrollBarHorz* scrollBar);

		/**	Triggers when the user scrolls the scroll bar. */
		void onScroll(float position, float size);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);

		typedef void(BS_THUNKCALL *OnScrolledThunkDef) (MonoObject*, float, MonoException**);
		static OnScrolledThunkDef onScrolledThunk;
	};

	/**	Interop class between C++ & CLR for GUIScrollBarV.  */
	class BS_SCR_BE_EXPORT ScriptGUIScrollBarV : public TScriptGUIElement<ScriptGUIScrollBarV>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUIScrollBarV")

	private:
		ScriptGUIScrollBarV(MonoObject* instance, GUIScrollBarVert* scrollBar);

		/**	Triggers when the user scrolls the scroll bar. */
		void onScroll(float position, float size);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);

		typedef void(BS_THUNKCALL *OnScrolledThunkDef) (MonoObject*, float, MonoException**);
		static OnScrolledThunkDef onScrolledThunk;
	};

	/**	Interop class between C++ & CLR for GUIResizeableScrollBarH.  */
	class BS_SCR_BE_EXPORT ScriptGUIResizeableScrollBarH : public TScriptGUIElement<ScriptGUIResizeableScrollBarH>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUIResizeableScrollBarH")

	private:
		ScriptGUIResizeableScrollBarH(MonoObject* instance, GUIScrollBarHorz* scrollBar);

		/**	Triggers when the user scrolls the scroll bar. */
		void onScroll(float position, float size);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);

		typedef void(BS_THUNKCALL *OnScrollOrResizeThunkDef) (MonoObject*, float, float, MonoException**);
		static OnScrollOrResizeThunkDef onScrollOrResizeThunk;
	};

	/**	Interop class between C++ & CLR for GUIResizeableScrollBarV.  */
	class BS_SCR_BE_EXPORT ScriptGUIResizeableScrollBarV : public TScriptGUIElement<ScriptGUIResizeableScrollBarV>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUIResizeableScrollBarV")

	private:
		ScriptGUIResizeableScrollBarV(MonoObject* instance, GUIScrollBarVert* scrollBar);

		/**	Triggers when the user scrolls the scroll bar. */
		void onScroll(float position, float size);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);

		typedef void(BS_THUNKCALL *OnScrollOrResizeThunkDef) (MonoObject*, float, float, MonoException**);
		static OnScrollOrResizeThunkDef onScrollOrResizeThunk;
	};

	/** @} */
}
