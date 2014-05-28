#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmGameObjectHandle.h"
#include "CmGameObjectManager.h"

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

		void onDeserializationEnded(IReflectable* obj)
		{
			GameObjectHandleBase* gameObjectHandle = static_cast<GameObjectHandleBase*>(obj);

			GameObjectManager::instance().registerUnresolvedHandle(*gameObjectHandle);
		}

		virtual const String& getRTTIName()
		{
			static String name = "GameObjectHandleBase";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GameObjectHandleBase;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			std::shared_ptr<GameObjectHandleBase> obj = cm_shared_ptr<GameObjectHandleBase, PoolAlloc>(new (cm_alloc<GameObjectHandleBase, PoolAlloc>()) GameObjectHandleBase());

			return obj;
		}
	};
}