#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace CamelotFramework
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

	private:
		friend class GameObjectHandleBase;
		friend class GameObjectManager;

		std::shared_ptr<GameObjectInstanceData> mInstanceData;

	protected:
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

namespace CamelotFramework
{
	// Game object handles
	typedef GameObjectHandle<GameObject> HGameObject;
	typedef GameObjectHandle<SceneObject> HSceneObject;
	typedef GameObjectHandle<Component> HComponent;
}