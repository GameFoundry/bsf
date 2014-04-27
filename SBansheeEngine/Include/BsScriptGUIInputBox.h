#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIInputBox : public ScriptObject<ScriptGUIInputBox>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIInputBox")

		GUIInputBox* getInternalValue() const { return mInputBox; }
		void* getNativeRaw() const { return mInputBox; }

	private:
		static void internal_createInstance(MonoObject* instance, bool multiline, MonoString* style, MonoArray* guiOptions);

		static void internal_setText(ScriptGUIInputBox* nativeInstance, MonoString* text);
		static void internal_getText(ScriptGUIInputBox* nativeInstance, MonoString** text);

		static void internal_destroy(ScriptGUIInputBox* nativeInstance);
		static void internal_setVisible(ScriptGUIInputBox* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIInputBox* nativeInstance, MonoObject* parentLayout);

		ScriptGUIInputBox(MonoObject* instance, GUIInputBox* inputBox);

		void destroy();
		void _onManagedInstanceDeleted();

		GUIInputBox* mInputBox;
		bool mIsDestroyed;
	};
}