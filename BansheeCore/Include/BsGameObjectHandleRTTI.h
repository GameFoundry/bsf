#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObjectHandle.h"
#include "BsGameObjectManager.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT GameObjectHandleRTTI : public RTTIType<GameObjectHandleBase, IReflectable, GameObjectHandleRTTI>
	{
	private:
		UINT64& getInstanceId(GameObjectHandleBase* obj)
		{
			static UINT64 invalidId = 0;

			if (obj->mData->mPtr != nullptr)
				return obj->mData->mPtr->mInstanceId;

			return invalidId;
		}

		void setInstanceId(GameObjectHandleBase* obj, UINT64& value) { obj->mRTTIData = value; } 

	public:
		GameObjectHandleRTTI()
		{
			addPlainField("mInstanceID", 0, &GameObjectHandleRTTI::getInstanceId, &GameObjectHandleRTTI::setInstanceId);
		}

		void onDeserializationEnded(IReflectable* obj) override
		{
			GameObjectHandleBase* gameObjectHandle = static_cast<GameObjectHandleBase*>(obj);

			UINT64 originalInstanceId = any_cast<UINT64>(gameObjectHandle->mRTTIData);
			GameObjectManager::instance().registerUnresolvedHandle(originalInstanceId, *gameObjectHandle);
			gameObjectHandle->mRTTIData = nullptr;
		}

		virtual const String& getRTTIName() override
		{
			static String name = "GameObjectHandleBase";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_GameObjectHandleBase;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			std::shared_ptr<GameObjectHandleBase> obj = bs_shared_ptr<GameObjectHandleBase>(new (bs_alloc<GameObjectHandleBase>()) GameObjectHandleBase());

			return obj;
		}
	};
}