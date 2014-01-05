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
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout, MonoArray* elements, MonoObject* style, MonoArray* guiOptions);
		static void internal_destroyInstance(ScriptGUIListBox* nativeInstance);
		static void internal_setElements(ScriptGUIListBox* nativeInstance, MonoArray* elements);

		static void internal_destroy(ScriptGUIListBox* nativeInstance);
		static void internal_disable(ScriptGUIListBox* nativeInstance);
		static void internal_enable(ScriptGUIListBox* nativeInstance);

		static void initRuntimeData();

		static void onSelectionChanged(MonoObject* instance, CM::UINT32 index);

		ScriptGUIListBox(GUIListBox* listBox);

		GUIListBox* mListBox;

		typedef void (__stdcall *OnSelectionChangedThunkDef) (MonoObject*, CM::UINT32, MonoException**);
		static OnSelectionChangedThunkDef onSelectionChangedThunk;
	};
}