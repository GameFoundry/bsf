#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class UndoRedo : public Module<UndoRedo>
	{
		struct GroupData
		{
			String name;
			UINT32 numEntries;
		};

	public:
		UndoRedo();
		~UndoRedo();

		void undo();
		void redo();

		void pushGroup(const String& name);
		void popGroup(const String& name);

		void registerCommand(EditorCommand* command);

	private:
		static const UINT32 MAX_STACK_ELEMENTS;

		EditorCommand** mUndoStack;
		EditorCommand** mRedoStack;

		UINT32 mUndoStackPtr;
		UINT32 mUndoNumElements;

		UINT32 mRedoStackPtr;
		UINT32 mRedoNumElements;

		Stack<GroupData> mGroups;

		EditorCommand* removeLastFromUndoStack();
		void addToUndoStack(EditorCommand* command);

		void clearUndoStack();
		void clearRedoStack();
	};
}
