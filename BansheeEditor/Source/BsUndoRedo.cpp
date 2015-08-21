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
		mUndoStack = bs_newN<EditorCommand*>(MAX_STACK_ELEMENTS);
		mRedoStack = bs_newN<EditorCommand*>(MAX_STACK_ELEMENTS);
	}

	UndoRedo::~UndoRedo()
	{
		clear();

		bs_deleteN(mUndoStack, MAX_STACK_ELEMENTS);
		bs_deleteN(mRedoStack, MAX_STACK_ELEMENTS);
	}

	void UndoRedo::undo()
	{
		if(mUndoNumElements == 0)
			return;

		EditorCommand* command = removeLastFromUndoStack();
		
		mRedoStackPtr = (mRedoStackPtr + 1) % MAX_STACK_ELEMENTS;
		mRedoStack[mRedoStackPtr] = command;
		mRedoNumElements = std::min(mRedoNumElements + 1, MAX_STACK_ELEMENTS);

		command->revert();
	}

	void UndoRedo::redo()
	{
		if(mRedoNumElements == 0)
			return;

		EditorCommand* command = mRedoStack[mRedoStackPtr];
		mRedoStackPtr = (mRedoStackPtr - 1) % MAX_STACK_ELEMENTS;
		mRedoNumElements--;

		addToUndoStack(command);

		command->commit();
	}

	void UndoRedo::pushGroup(const String& name)
	{
		mGroups.push(GroupData());
		GroupData& newGroup = mGroups.top();

		newGroup.name = name;
		newGroup.numEntries = 0;

		clearRedoStack();
	}

	void UndoRedo::popGroup(const String& name)
	{
		if(mGroups.empty())
			BS_EXCEPT(InvalidStateException, "Attempting to pop an UndoRedo group that doesn't exist: " + name);

		GroupData& topGroup = mGroups.top();
		if(topGroup.name != name)
			BS_EXCEPT(InvalidStateException, "Attempting to pop invalid UndoRedo group. Got: " + name + ". Expected: " + topGroup.name);

		for(UINT32 i = 0; i < topGroup.numEntries; i++)
		{
			EditorCommand* command = mUndoStack[mUndoStackPtr];
			mUndoStackPtr = (mUndoStackPtr - 1) % MAX_STACK_ELEMENTS;
			mUndoNumElements--;

			EditorCommand::destroy(command);
		}

		mGroups.pop();
		clearRedoStack();
	}

	void UndoRedo::registerCommand(EditorCommand* command)
	{
		addToUndoStack(command);

		clearRedoStack();
	}

	void UndoRedo::clear()
	{
		clearUndoStack();
		clearRedoStack();
	}

	EditorCommand* UndoRedo::removeLastFromUndoStack()
	{
		EditorCommand* command = mUndoStack[mUndoStackPtr];
		mUndoStackPtr = (mUndoStackPtr - 1) % MAX_STACK_ELEMENTS;
		mUndoNumElements--;

		if(!mGroups.empty())
		{
			GroupData& topGroup = mGroups.top();

			if(topGroup.numEntries == 0)
			{
				BS_EXCEPT(InvalidStateException, "Removing an element from UndoRedo stack while in an " \
					"invalid UndoRedo group. Current group: " + topGroup.name);
			}

			topGroup.numEntries--;
		}

		return command;
	}

	void UndoRedo::addToUndoStack(EditorCommand* command)
	{
		mUndoStackPtr = (mUndoStackPtr + 1) % MAX_STACK_ELEMENTS;
		mUndoStack[mUndoStackPtr] = command;
		mUndoNumElements = std::min(mUndoNumElements + 1, MAX_STACK_ELEMENTS);

		if(!mGroups.empty())
		{
			GroupData& topGroup = mGroups.top();
			topGroup.numEntries = std::min(topGroup.numEntries + 1, MAX_STACK_ELEMENTS);
		}
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

		while(!mGroups.empty())
			mGroups.pop();
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