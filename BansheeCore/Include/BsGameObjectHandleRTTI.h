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
		UINT64& getInstanceId(GameObjectHandleBase* obj) { return obj->mData->mInstanceId; }
		void setInstanceId(GameObjectHandleBase* obj, UINT64& value) { obj->mData->mInstanceId = value; } 

	public:
		GameObjectHandleRTTI()
		{
			addPlainField("mInstanceID", 0, &GameObjectHandleRTTI::getInstanceId, &GameObjectHandleRTTI::setInstanceId);
		}

		void onDeserializationEnded(IReflectable* obj) override
		{
			GameObjectHandleBase* gameObjectHandle = static_cast<GameObjectHandleBase*>(obj);

			GameObjectManager::instance().registerUnresolvedHandle(*gameObjectHandle);
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