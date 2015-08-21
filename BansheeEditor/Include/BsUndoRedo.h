#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles editor-wide undo & redo operations.
	 */
	class BS_ED_EXPORT UndoRedo : public Module<UndoRedo>
	{
		/**
		 * @brief	Contains data about a single undo/redo group.
		 *			Groups allow you to create context sensitive undo/redo operations.
		 */
		struct GroupData
		{
			String name;
			UINT32 numEntries;
		};

	public:
		UndoRedo();
		~UndoRedo();

		/**
		 * @brief	Executes the last command on the undo stack, undoing its operations.
		 */
		void undo();

		/**
		 * @brief	Executes the last command on the redo stack (last command we called undo on), 
		 *			re-applying its operation.
		 */
		void redo();

		/**
		 * @brief	Creates a new undo/redo group. All new commands will be registered to this group.
		 *			You may remove the group and all of its commands by calling ::popGroup.
		 *
		 *			For example you might require global editor-wide undo/redo operations, and also
		 *			more specific ones like input in an input box. When the user is done with the input
		 *			box you no longer require its undo operations and you may use groups to easily
		 *			remove them.
		 *
		 * @param	name	Unique name for the group.
		 */
		void pushGroup(const String& name);

		/**
		 * @brief	Removes all the command registered to the current undo/redo group.
		 *
		 * @param	name	Unique name for the group.
		 */
		void popGroup(const String& name);

		/**
		 * @brief	Registers a new undo command.
		 */
		void registerCommand(EditorCommand* command);

		/**
		 * @brief	Resets the undo/redo stacks.
		 */
		void clear();

	private:
		/**
		 * @brief	Removes the last undo command from the undo stack, and returns it.
		 */
		EditorCommand* removeLastFromUndoStack();

		/**
		 * @brief	Adds a new command to the undo stack.
		 */
		void addToUndoStack(EditorCommand* command);

		/**
		 * @brief	Removes all entries from the undo stack.
		 */
		void clearUndoStack();

		/**
		 * @brief	Removes all entries from the redo stack.
		 */
		void clearRedoStack();

		static const UINT32 MAX_STACK_ELEMENTS;

		EditorCommand** mUndoStack;
		EditorCommand** mRedoStack;

		UINT32 mUndoStackPtr;
		UINT32 mUndoNumElements;

		UINT32 mRedoStackPtr;
		UINT32 mRedoNumElements;

		Stack<GroupData> mGroups;
	};
}
