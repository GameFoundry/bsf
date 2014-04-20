#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace BansheeEngine
{
	struct GameObjectInstanceData
	{
		GameObjectInstanceData()
			:mInstanceId(0), object(nullptr)
		{ }

		std::shared_ptr<GameObject> object;
		UINT64 mInstanceId;
	};

	class CM_EXPORT GameObject : public IReflectable
	{
	public:
		GameObject();
		virtual ~GameObject();

		UINT64 getInstanceId() const { return mInstanceData->mInstanceId; }

		const String& getName() const { return mName; }
		void setName(const String& name) { mName = name; }

	private:
		friend class GameObjectHandleBase;
		friend class GameObjectManager;

		std::shared_ptr<GameObjectInstanceData> mInstanceData;

	protected:
		String mName;

		void initialize(const std::shared_ptr<GameObject>& object, UINT64 instanceId);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class GameObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}

#include "CmGameObjectHandle.h"

namespace BansheeEngine
{
	// Game object handles
	typedef GameObjectHandle<GameObject> HGameObject;
	typedef GameObjectHandle<SceneObject> HSceneObject;
	typedef GameObjectHandle<Component> HComponent;
}