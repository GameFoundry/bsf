#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"

namespace BansheeEngine
{
	// TODO - This is only valid for plain field types. Add something similar for pointers and/or arrays?
	/**
	 * @brief	A command used for undo/redo purposes. It records a value of a single RTTI field and
	 *			using the provided interface allows you to commit a change to that field, or restore
	 *			it to the original value.
	 */
	template<class T>
	class CmdEditPlainFieldGO : public EditorCommand
	{
	public:
		~CmdEditPlainFieldGO() 
		{
			if(mNewData != nullptr)
				bs_free(mNewData);

			if(mOldData != nullptr)
				bs_free(mOldData);
		}

		/**
		 * @brief	Creates and executes the command on the provided object and field.
		 *			Automatically registers the command with undo/redo system.
		 *
		 * @param	gameObject	Game object to search for the field.
		 * @param	fieldName	RTTI name of the field.
		 * @param	fieldValue	New value for the field.
		 */
		static void execute(const GameObjectHandleBase& gameObject, const String& fieldName, const T& fieldValue)
		{
			// Register command and commit it
			CmdEditPlainFieldGO* command = new (bs_alloc<CmdEditPlainFieldGO>()) CmdEditPlainFieldGO(gameObject, fieldName, fieldValue);
			UndoRedo::instance().registerCommand(command);
			command->commit();
		}

		/**
		 * @copydoc	EditorCommand::commit
		 */
		void commit() override
		{
			if(mGameObject.isDestroyed())
				return;

			T fieldValue;
			RTTIPlainType<T>::fromMemory(fieldValue, (char*)mNewData);

			RTTITypeBase* rtti = mGameObject->getRTTI();
			rtti->setPlainValue(mGameObject.get(), mFieldName, fieldValue);
		}

		/**
		 * @copydoc	EditorCommand::revert
		 */
		void revert() override
		{
			if(mGameObject.isDestroyed())
				return;

			T fieldValue;
			RTTIPlainType<T>::fromMemory(fieldValue, (char*)mOldData);

			RTTITypeBase* rtti = mGameObject->getRTTI();
			rtti->setPlainValue(mGameObject.get(), mFieldName, fieldValue);
		}

	private:
		friend class UndoRedo;

		CmdEditPlainFieldGO(const GameObjectHandleBase& gameObject, const String& fieldName, const T& fieldValue)
			:mGameObject(gameObject), mFieldName(fieldName), mNewData(nullptr), mOldData(nullptr)
		{
			// Convert new value to bytes
			UINT32 newDataNumBytes = RTTIPlainType<T>::getDynamicSize(fieldValue);
			mNewData = bs_alloc(newDataNumBytes);

			RTTIPlainType<T>::toMemory(fieldValue, (char*)mNewData);

			// Get old value and also convert it to bytes
			String oldFieldValue;
			gameObject->getRTTI()->getPlainValue(gameObject.get(), fieldName, oldFieldValue);

			UINT32 oldDataNumBytes = RTTIPlainType<T>::getDynamicSize(oldFieldValue);
			mOldData = bs_alloc(oldDataNumBytes);

			RTTIPlainType<T>::toMemory(oldFieldValue, (char*)mOldData);
		}

		GameObjectHandleBase mGameObject;
		String mFieldName;

		void* mNewData;
		void* mOldData;
	};
}