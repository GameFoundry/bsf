#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace CamelotFramework
{
	class CM_EXPORT GameObject : public IReflectable
	{
	public:
		GameObject();
		virtual ~GameObject();

		UINT64 getInstanceID() const { return mInstanceId; }

	private:
		UINT64 mInstanceId;

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