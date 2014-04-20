#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIListBox : public ScriptObject<ScriptGUIListBox>
	{
	public:
		static void initMetaData();

		GUIListBox* getInternalValue() const { return mListBox; }
		void* getNativeRaw() const { return mListBox; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoArray* elements, MonoString* style, MonoArray* guiOptions);
		static void internal_destroyInstance(ScriptGUIListBox* nativeInstance);
		static void internal_setElements(ScriptGUIListBox* nativeInstance, MonoArray* elements);

		static void internal_destroy(ScriptGUIListBox* nativeInstance);
		static void internal_setVisible(ScriptGUIListBox* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIListBox* nativeInstance, MonoObject* parentLayout);

		static void initRuntimeData();

		static void onSelectionChanged(MonoObject* instance, UINT32 index);

		ScriptGUIListBox(GUIListBox* listBox);

		void destroy();

		GUIListBox* mListBox;
		bool mIsDestroyed;

		typedef void (__stdcall *OnSelectionChangedThunkDef) (MonoObject*, UINT32, MonoException**);
		static OnSelectionChangedThunkDef onSelectionChangedThunk;
	};
}