#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"

namespace BansheeEditor
{
	// TODO - This is only valid for plain field types. Add something similar for pointers and/or arrays?
	template<class T>
	class CmdEditPlainFieldGO : public EditorCommand
	{
	public:
		~CmdEditPlainFieldGO() 
		{
			if(mNewData != nullptr)
				cm_free(mNewData);

			if(mOldData != nullptr)
				cm_free(mOldData);
		}

		void execute(const CM::GameObjectHandleBase& gameObject, const CM::String& fieldName, const T& fieldValue)
		{
			// Register command and commit it
			CmdEditFieldGO* command = cm_new<CmdEditFieldGO>(gameObject, fieldName, fieldValue);
			UndoRedo::instance().registerCommand(command);
			command->commit();
		}

		void commit()
		{
			if(mGameObject.isDestroyed())
				return;

			T fieldValue;
			CM::RTTIPlainType<T>::fromMemory(fieldValue, (char*)mNewData);

			CM::RTTITypeBase* rtti = mGameObject->getRTTI();
			rtti->setPlainValue(mGameObject.get(), mFieldName, fieldValue);
		}

		void revert()
		{
			if(mGameObject.isDestroyed())
				return;

			T fieldValue;
			CM::RTTIPlainType<T>::fromMemory(fieldValue, (char*)mOldData);

			CM::RTTITypeBase* rtti = mGameObject->getRTTI();
			rtti->setPlainValue(mGameObject.get(), mFieldName, fieldValue);
		}

	private:
		friend class UndoRedo;

		CmdEditPlainFieldGO(const CM::GameObjectHandleBase& gameObject, const CM::String& fieldName, const T& fieldValue)
			:mGameObject(gameObject), mFieldName(fieldName), mNewData(nullptr), mOldData(nullptr)
		{
			// Convert new value to bytes
			CM::UINT32 newDataNumBytes = CM::RTTIPlainType<T>::getDynamicSize(fieldValue);
			mNewData = CM::cm_alloc(newDataNumBytes);

			CM::RTTIPlainType<T>::toMemory(fieldValue, (char*)mNewData);

			// Get old value and also convert it to bytes
			CM::String oldFieldValue;
			gameObject->getRTTI()->getPlainValue(gameObject.get(), fieldName, oldFieldValue);

			CM::UINT32 oldDataNumBytes = CM::RTTIPlainType<T>::getDynamicSize(oldFieldValue);
			mOldData = cm_alloc(oldDataNumBytes);

			CM::RTTIPlainType<T>::toMemory(oldFieldValue, (char*)mOldData);
		}

		CM::GameObjectHandleBase mGameObject;
		CM::String mFieldName;

		void* mNewData;
		void* mOldData;
	};
}