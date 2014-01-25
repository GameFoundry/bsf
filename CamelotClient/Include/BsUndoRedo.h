#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"

namespace BansheeEditor
{
	class UndoRedo : public CM::Module<UndoRedo>
	{
	public:
		UndoRedo();
		~UndoRedo();

		void undo();
		void redo();

		void registerCommand(EditorCommand* command);

	private:
		static const CM::UINT32 MAX_STACK_ELEMENTS;

		EditorCommand** mUndoStack;
		EditorCommand** mRedoStack;

		CM::UINT32 mUndoStackPtr;
		CM::UINT32 mUndoNumElements;

		CM::UINT32 mRedoStackPtr;
		CM::UINT32 mRedoNumElements;

		void clearUndoStack();
		void clearRedoStack();
	};
}
