#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"

namespace BansheeEngine
{
	// TODO - This is only valid for plain field types. Add something similar for pointers and/or arrays?
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

		static void execute(const GameObjectHandleBase& gameObject, const String& fieldName, const T& fieldValue)
		{
			// Register command and commit it
			CmdEditPlainFieldGO* command = new (bs_alloc<CmdEditPlainFieldGO>()) CmdEditPlainFieldGO(gameObject, fieldName, fieldValue);
			UndoRedo::instance().registerCommand(command);
			command->commit();
		}

		void commit()
		{
			if(mGameObject.isDestroyed())
				return;

			T fieldValue;
			RTTIPlainType<T>::fromMemory(fieldValue, (char*)mNewData);

			RTTITypeBase* rtti = mGameObject->getRTTI();
			rtti->setPlainValue(mGameObject.get(), mFieldName, fieldValue);
		}

		void revert()
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