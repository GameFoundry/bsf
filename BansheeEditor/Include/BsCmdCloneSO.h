#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"
#include "BsCmdUtility.h"

namespace BansheeEngine
{
	/**
	 * @brief	A command used for undo/redo purposes. Clones scene object(s)
	 *			and removes them as an undo operation.
	 */
	class BS_ED_EXPORT CmdCloneSO : public EditorCommand
	{
	public:
		~CmdCloneSO();

		/**
		 * @brief	Creates a new scene object by cloning an existing object.
		 *			Automatically registers the command with undo/redo system.
		 *
		 * @param	sceneObject		Scene object to clone.
		 * @param	description		Optional description of what exactly the command does.
		 *
		 * @return	Cloned object.
		 */
		static HSceneObject execute(const HSceneObject& sceneObject, const WString& description = StringUtil::WBLANK);

		/**
		 * @brief	Creates new scene object(s) by cloning existing objects.
		 *			Automatically registers the command with undo/redo system.
		 *
		 * @param	sceneObjects	Scene object(s) to clone.
		 * @param	description		Optional description of what exactly the command does.
		 *
		 * @return	Cloned objects.
		 */
		static Vector<HSceneObject> execute(const Vector<HSceneObject>& sceneObjects, const WString& description = StringUtil::WBLANK);

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

		CmdCloneSO(const WString& description, const Vector<HSceneObject>& originals);

		Vector<HSceneObject> mOriginals;
		Vector<HSceneObject> mClones;
	};
}