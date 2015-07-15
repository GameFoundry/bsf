#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	A command used for undo/redo purposes. It records a change occurring on
	 *			some object and allows you to apply or revert that change as needed.
	 */
	class EditorCommand
	{
	public:
		virtual ~EditorCommand() { }

		/**
		 * @brief	Applies the command, commiting the change.
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
	};
}