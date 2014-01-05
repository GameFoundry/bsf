#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIInputBox : public ScriptObject<ScriptGUIInputBox>
	{
	public:
		static void initMetaData();

		GUIInputBox* getInternalValue() const { return mInputBox; }
		void* getNativeRaw() const { return mInputBox; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout, bool multiline, MonoObject* style, MonoArray* guiOptions);
		static void internal_destroyInstance(ScriptGUIInputBox* nativeInstance);

		static void internal_setText(ScriptGUIInputBox* nativeInstance, MonoString* text);
		static void internal_getText(ScriptGUIInputBox* nativeInstance, MonoString** text);

		static void internal_destroy(ScriptGUIInputBox* nativeInstance);
		static void internal_disable(ScriptGUIInputBox* nativeInstance);
		static void internal_enable(ScriptGUIInputBox* nativeInstance);

		static void initRuntimeData();

		ScriptGUIInputBox(GUIInputBox* inputBox);

		GUIInputBox* mInputBox;
	};
}