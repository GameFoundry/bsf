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

	typedef std::shared_ptr<GameObjectInstanceData> GameObjectInstanceDataPtr;

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
		 * @brief	Returns an ID that identifies a link between this object and its equivalent
		 *			in the linked prefab. This will be -1 if the object has no prefab link, or if
		 *			the object is specific to the instance and has no prefab equivalent.
		 */
		INT32 getLinkId() const { return mLinkId; }

		/**
		 * @brief	Gets the name of the object.
		 */
		const String& getName() const { return mName; }

		/**
		 * @brief	Sets the name of the object.
		 */
		void setName(const String& name) { mName = name; }

		/**
		 * @brief	Marks the object as destroyed. Generally this means the object
		 *			has been queued for destruction but it hasn't occurred yet.
		 *
		 * @note	Internal method.
		 */
		void _setIsDestroyed() { mIsDestroyed = true; }

		/**
		 * @brief	Checks if the object has been destroyed.
		 */
		bool _getIsDestroyed() const { return mIsDestroyed; }

		/**
		 * @brief	Replaces the instance data with another objects instance data.
		 *			This object will basically become the original owner of the provided
		 *			instance data as far as all game object handles referencing it are concerned.
		 *
		 * @note	Internal method. 
		 *			No alive objects should ever be sharing the same instance data. This can be used
		 *			for restoring dead handles.
		 */
		virtual void _setInstanceData(GameObjectInstanceDataPtr& other);

		/**
		 * @brief	Returns instance data that identifies this GameObject and is used for referencing
		 *			by game object handles.
		 *
		 * @note	Internal method.
		 */
		virtual GameObjectInstanceDataPtr _getInstanceData() const { return mInstanceData; }

	protected:
		friend class GameObjectHandleBase;
		friend class GameObjectManager;

		/**
		 * @brief	Initializes the GameObject after construction.
		 */
		void initialize(const std::shared_ptr<GameObject>& object, UINT64 instanceId);

	protected:
		String mName;
		INT32 mLinkId;

	private:
		friend class Prefab;

		GameObjectInstanceDataPtr mInstanceData;
		bool mIsDestroyed;

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