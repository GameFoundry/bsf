#include "BsUndoRedo.h"
#include "BsEditorCommand.h"

namespace BansheeEngine
{
	const UINT32 UndoRedo::MAX_STACK_ELEMENTS = 1000;

	UndoRedo::UndoRedo()
		:mUndoStackPtr(0), mUndoNumElements(0),
		mRedoStackPtr(0), mRedoNumElements(0),
		mUndoStack(nullptr), mRedoStack(nullptr)
	{
		mUndoStack = cm_newN<EditorCommand*>(MAX_STACK_ELEMENTS);
		mRedoStack = cm_newN<EditorCommand*>(MAX_STACK_ELEMENTS);
	}

	UndoRedo::~UndoRedo()
	{
		clearUndoStack();
		clearRedoStack();

		cm_deleteN(mUndoStack, MAX_STACK_ELEMENTS);
		cm_deleteN(mRedoStack, MAX_STACK_ELEMENTS);
	}

	void UndoRedo::undo()
	{
		if(mUndoNumElements == 0)
			return;

		EditorCommand* command = mUndoStack[mUndoStackPtr];
		mUndoStackPtr = (mUndoStackPtr - 1) % MAX_STACK_ELEMENTS;
		mUndoNumElements--;

		mRedoStackPtr = (mRedoStackPtr + 1) % MAX_STACK_ELEMENTS;
		mRedoStack[mRedoStackPtr] = command;
		mRedoNumElements++;

		command->revert();
	}

	void UndoRedo::redo()
	{
		if(mRedoNumElements == 0)
			return;

		EditorCommand* command = mRedoStack[mRedoStackPtr];
		mRedoStackPtr = (mRedoStackPtr - 1) % MAX_STACK_ELEMENTS;
		mRedoNumElements--;

		mUndoStackPtr = (mUndoStackPtr + 1) % MAX_STACK_ELEMENTS;
		mUndoStack[mUndoStackPtr] = command;
		mUndoNumElements++;

		command->commit();
	}

	void UndoRedo::registerCommand(EditorCommand* command)
	{
		mUndoStackPtr = (mUndoStackPtr + 1) % MAX_STACK_ELEMENTS;
		mUndoStack[mUndoStackPtr] = command;
		mUndoNumElements++;

		clearRedoStack();
	}

	void UndoRedo::clearUndoStack()
	{
		while(mUndoNumElements > 0)
		{
			EditorCommand* command = mUndoStack[mUndoStackPtr];
			mUndoStackPtr = (mUndoStackPtr - 1) % MAX_STACK_ELEMENTS;
			mUndoNumElements--;

			EditorCommand::destroy(command);
		}
	}

	void UndoRedo::clearRedoStack()
	{
		while(mRedoNumElements > 0)
		{
			EditorCommand* command = mRedoStack[mRedoStackPtr];
			mRedoStackPtr = (mRedoStackPtr - 1) % MAX_STACK_ELEMENTS;
			mRedoNumElements--;

			EditorCommand::destroy(command);
		}
	}
}