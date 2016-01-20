//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"
#include "BsCmdUtility.h"

namespace BansheeEngine
{
	/**
	 * @brief	A command used for undo/redo purposes. Instantiates scene object(s)
	 *			from a prefab and removes them as an undo operation.
	 */
	class BS_ED_EXPORT CmdInstantiateSO : public EditorCommand
	{
	public:
		~CmdInstantiateSO();

		/**
		 * @brief	Instantiates the specified prefab.
		 *			Automatically registers the command with undo/redo system.
		 *
		 * @param	prefab			Prefab to instantiate.
		 * @param	description		Optional description of what exactly the command does.
		 *
		 * @return	Instantiated object.
		 */
		static HSceneObject execute(const HPrefab& prefab, const WString& description = StringUtil::WBLANK);

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

		CmdInstantiateSO(const WString& description, const HPrefab& prefab);

		HPrefab mPrefab;
		HSceneObject mSceneObject;
	};
}