#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	A command used for undo/redo purposes. It records a change occurring on
	 *			some object and allows you to apply or revert that change as needed.
	 */
	class BS_ED_EXPORT EditorCommand
	{
	public:
		EditorCommand(const WString& description);
		virtual ~EditorCommand() { }

		/**
		 * @brief	Applies the command, committing the change.
		 */
		virtual void commit() { }

		/**
		 * @brief	Reverts the command, reverting the change previously
		 *			done with "commit".
		 */
		virtual void revert() { }

		/**
		 * @brief	Deletes the command.
		 */
		static void destroy(EditorCommand* command);

	private:
		friend class UndoRedo;

		WString mDescription;
		UINT32 mId;
	};
}