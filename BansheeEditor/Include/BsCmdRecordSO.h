#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"

namespace BansheeEngine
{
	/**
	 * @brief	A command used for undo/redo purposes. It records a state of the entire
	 *			scene object at a specific point and allows you to restore it to its
	 *			original values as needed.
	 */
	class CmdRecordSO : public EditorCommand
	{
		/**
		 * @brief	Contains stored information about stored scene object instance data,
		 *			including all of its children and components.
		 *
		 * @note	When object is serialized it will receive new instance data (as if it was a new
		 *			object). But we want to restore the original object completely (including any references
		 *			other objects might have to it) so we need store the instance data.
		 */
		struct SceneObjProxy
		{
			GameObjectInstanceDataPtr instanceData;

			Vector<GameObjectInstanceDataPtr> componentInstanceData;
			Vector<SceneObjProxy> children;
		};

	public:
		~CmdRecordSO();

		/**
		 * @brief	Creates and executes the command on the provided scene object.
		 *			Automatically registers the command with undo/redo system.
		 *
		 * @param	inputField	Input field to modify the value on.
		 * @param	value		New value for the field.
		 */
		static void execute(const HSceneObject& sceneObject);

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

		CmdRecordSO(const HSceneObject& sceneObject);

		/**
		 * @brief	Saves the state of the specified object, all of its children
		 *			and components. Make sure to call "clear" when you no longer need
		 *			the data, or wish to call this method again.
		 */
		void recordSO(const HSceneObject& sceneObject);

		/**
		 * @brief	Clears all the stored data and frees memory.
		 */
		void clear();

		/**
		 * @brief	Parses the scene object hierarchy and components and generates a
		 *			hierarchy of instance data required to restore the object identities.
		 *
		 * @see		SceneObjProxy
		 */
		SceneObjProxy createProxy(const HSceneObject& sceneObject);

		/**
		 * @brief	Restores original object instance data after deserialization.
		 *
		 * @see		SceneObjProxy
		 */
		void restoreIds(const HSceneObject& restored);

		HSceneObject mSceneObject;
		SceneObjProxy mSceneObjectProxy;

		UINT8* mSerializedObject;
		UINT32 mSerializedObjectSize;
		UINT64 mSerializedObjectParentId;
	};
}