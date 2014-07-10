//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
			std::shared_ptr<GameObjectHandleBase> obj = bs_shared_ptr<GameObjectHandleBase, PoolAlloc>(new (bs_alloc<GameObjectHandleBase, PoolAlloc>()) GameObjectHandleBase());

			return obj;
		}
	};
}