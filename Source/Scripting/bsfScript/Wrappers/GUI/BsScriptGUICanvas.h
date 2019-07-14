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

	/**	Interop class between C++ & CLR for GUICanvas. */
	class BS_SCR_BE_EXPORT ScriptGUICanvas : public TScriptGUIElement<ScriptGUICanvas>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUICanvas")

	private:
		ScriptGUICanvas(MonoObject* instance, GUICanvas* canvas);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_drawLine(ScriptGUICanvas* nativeInstance, Vector2I* a, Vector2I* b, Color* color, UINT8 depth);
		static void internal_drawPolyLine(ScriptGUICanvas* nativeInstance, MonoArray* vertices, Color* color, UINT8 depth);
		static void internal_drawTexture(ScriptGUICanvas* nativeInstance, ScriptSpriteTexture* texture, Rect2I* area,
			TextureScaleMode scaleMode, Color* color, UINT8 depth);
		static void internal_drawTriangleStrip(ScriptGUICanvas* nativeInstance, MonoArray* vertices, Color* color, UINT8 depth);
		static void internal_drawTriangleList(ScriptGUICanvas* nativeInstance, MonoArray* vertices, Color* color, UINT8 depth);
		static void internal_drawText(ScriptGUICanvas* nativeInstance, MonoString* text, Vector2I* position,
			ScriptFont* font, UINT32 size, Color* color, UINT8 depth);
		static void internal_clear(ScriptGUICanvas* nativeInstance);
	};

	/** @} */
}
