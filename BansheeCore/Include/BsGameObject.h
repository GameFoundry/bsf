#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains instance data that is held by all GameObject handles.
	 */
	struct GameObjectInstanceData
	{
		GameObjectInstanceData()
			:mInstanceId(0), object(nullptr)
		{ }

		std::shared_ptr<GameObject> object;
		UINT64 mInstanceId;
	};

	/**
	 * @brief	Type of object that can be referenced by a GameObject handle.
	 *			Each object has an unique ID and is registered with the GameObjectManager.
	 */
	class BS_CORE_EXPORT GameObject : public IReflectable
	{
	public:
		GameObject();
		virtual ~GameObject();

		/**
		 * @brief	Returns the unique instance ID of the GameObject.
		 */
		UINT64 getInstanceId() const { return mInstanceData->mInstanceId; }

		/**
		 * @brief	Gets the name of the object.
		 */
		const String& getName() const { return mName; }

		/**
		 * @brief	Sets the name of the object.
		 */
		void setName(const String& name) { mName = name; }

	protected:
		friend class GameObjectHandleBase;
		friend class GameObjectManager;

		/**
		 * @brief	Initializes the GameObject after construction.
		 */
		void initialize(const std::shared_ptr<GameObject>& object, UINT64 instanceId);

	protected:
		String mName;

	private:
		std::shared_ptr<GameObjectInstanceData> mInstanceData;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class GameObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}

#include "BsGameObjectHandle.h"

namespace BansheeEngine
{
	// Game object handles
	typedef GameObjectHandle<GameObject> HGameObject;
	typedef GameObjectHandle<SceneObject> HSceneObject;
	typedef GameObjectHandle<Component> HComponent;
}