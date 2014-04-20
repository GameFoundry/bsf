#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class UndoRedo : public Module<UndoRedo>
	{
	public:
		UndoRedo();
		~UndoRedo();

		void undo();
		void redo();

		void registerCommand(EditorCommand* command);

	private:
		static const UINT32 MAX_STACK_ELEMENTS;

		EditorCommand** mUndoStack;
		EditorCommand** mRedoStack;

		UINT32 mUndoStackPtr;
		UINT32 mUndoNumElements;

		UINT32 mRedoStackPtr;
		UINT32 mRedoNumElements;

		void clearUndoStack();
		void clearRedoStack();
	};
}
