#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"
#include "BsCmdUtility.h"

namespace BansheeEngine
{
	/**
	 * @brief	A command used for undo/redo purposes. Creates a scene object 
	 *			and removes it as an undo operation.
	 */
	class CmdCreateSO : public EditorCommand
	{
	public:
		~CmdCreateSO();

		/**
		 * @brief	Creates a new scene object.
		 *			Automatically registers the command with undo/redo system.
		 *
		 * @param	name		Name of the scene object.
		 * @param	flags		Optional creation flags for the scene object.
		 * @param	description	Optional description of what exactly the command does.
		 *
		 * @return	Newly created scene object.
		 */
		static HSceneObject execute(const String& name, UINT32 flags, const WString& description = StringUtil::WBLANK);

		/**
		 * @copydoc	EditorCommand::commit
		 */
		void commit() override;

		/**
		 * @copydoc	EditorCommand::revert
		 */
		void revert() override;

	private:
		friend class UndoRedo;

		CmdCreateSO(const WString& description, const String& name, UINT32 flags);

		String mName;
		UINT32 mFlags;

		HSceneObject mSceneObject;
	};
}