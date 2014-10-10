#pragma once

#include "BsCorePrerequisites.h"
#include "BsMatrix4.h"
#include "BsVector3.h"
#include "BsQuaternion.h"
#include "BsRTTIType.h"
#include "BsCoreSceneManager.h"
#include "BsGameObjectManager.h"
#include "BsGameObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	SceneObject represents an object in the scene graph. It has a world position,
	 *			place in the hierarchy and optionally a number of attached components.
	 */
	class BS_CORE_EXPORT SceneObject : public GameObject
	{
		/**
		 * @brief	Flags that signify which part of the SceneObject needs updating.
		 */
		enum DirtyFlags
		{
			LocalTfrmDirty = 0x01,
			WorldTfrmDirty = 0x02
		};

		friend class CoreSceneManager;
	public:
		~SceneObject();

		/**
		 * @brief	Creates a new SceneObject with the specified name. Object will be placed in the top
		 *			of the scene hierarchy.
		 */
		static HSceneObject create(const String& name);

		/**
		 * @brief	Destroys this object and any of its held components.
		 */
		void destroy();

		/**
		 * @copydoc	GameObject::_setInstanceData
		 */
		void _setInstanceData(GameObjectInstanceDataPtr& other);

		/**
		 * @brief	Returns a handle to this object.
		 */
		HSceneObject getHandle() const { return mThisHandle; }
	private:
		SceneObject(const String& name);

		static HSceneObject createInternal(const String& name);
		void destroyInternal();

	private:
		HSceneObject mThisHandle;

		/************************************************************************/
		/* 								Transform	                     		*/
		/************************************************************************/
	public:
		/**
		 * @brief	Sets the local position of the object.
		 */
		void setPosition(const Vector3& position);

		/**
		 * @brief	Gets the local position of the object.
		 */
		const Vector3& getPosition() const { return mPosition; }

		/**
		 * @brief	Sets the world position of the object.
		 */
		void setWorldPosition(const Vector3& position);

		/**
		 * @brief	Gets the world position of the object.
		 *
		 * @note	Performance warning: This might involve updating the transforms if the transform is dirty.
		 */
		const Vector3& getWorldPosition() const;

		/**
		 * @brief	Sets the local rotation of the object.
		 */
		void setRotation(const Quaternion& rotation);

		/**
		 * @brief	Gets the local rotation of the object.
		 */
		const Quaternion& getRotation() const { return mRotation; }

		/**
		 * @brief	Sets the world rotation of the object.
		 */
		void setWorldRotation(const Quaternion& rotation);

		/**
		 * @brief	Gets world rotation of the object.
		 *
		 * @note	Performance warning: This might involve updating the transforms if the transform is dirty.
		 */
		const Quaternion& getWorldRotation() const;

		/**
		 * @brief	Sets the local scale of the object.
		 */
		void setScale(const Vector3& scale);

		/**
		 * @brief	Gets the local scale of the object.
		 */
		const Vector3& getScale() const { return mScale; }

		/**
		 * @brief	Gets world scale of the object.
		 *
		 * @note	Performance warning: This might involve updating the transforms if the transform is dirty.
		 */
		const Vector3& getWorldScale() const;

		/**
		 * @brief	Orients the object so it is looking at the provided "location" (local space)
		 *			where "up" is used for determining the location of the objects Y axis.
		 *
		 */
		void lookAt(const Vector3& location, const Vector3& up = Vector3::UNIT_Y);

		/**
		 * @brief	Gets the objects world transform matrix.
		 *
		 * @note	Performance warning: This might involve updating the transforms if the transform is dirty.
		 */
		const Matrix4& getWorldTfrm() const;

		/**
		 * @brief	Gets the objects local transform matrix.
		 */
		const Matrix4& getLocalTfrm() const;

		/**
		 * @brief	Moves the object's position by the vector offset provided along world axes.
		 */
        void move(const Vector3& vec);

		/**
		 * @brief	Moves the object's position by the vector offset provided along it's own axes (relative to orientation).
		 */
        void moveRelative(const Vector3& vec);

		/**
		 * @brief	Gets the Z (forward) axis of the object, in world space.
		 *
		 * @return	Forward axis of the object.
		 */
		Vector3 getForward() const { return getWorldRotation().rotate(-Vector3::UNIT_Z); }

		/**
		 * @brief	Gets the Y (up) axis of the object, in world space.
		 *
		 * @return	Up axis of the object.
		 */
		Vector3 getUp() const { return getWorldRotation().rotate(Vector3::UNIT_Y); }

		/**
		 * @brief	Gets the X (right) axis of the object, in world space.
		 *
		 * @return	Right axis of the object.
		 */
		Vector3 getRight() const { return getWorldRotation().rotate(Vector3::UNIT_X); }

		/**
		 * @brief	Rotates the game object so it's forward axis faces the provided
		 * 			direction.
		 * 			
		 * @note	Local forward axis is considered to be negative Z.
		 *
		 * @param	forwardDir	The forward direction to face, in world space.
		 */
		void setForward(const Vector3& forwardDir);

		/**
		 * @brief	Rotate the object around an arbitrary axis.
		 */
        void rotate(const Vector3& axis, const Radian& angle);

		/**
		 * @brief	Rotate the object around an arbitrary axis using a Quaternion.
		 */
        void rotate(const Quaternion& q);

		/**
		 * @brief	Rotates around local Z axis.
		 *
		 * @param	angle	Angle to rotate by.
		 */
		void roll(const Radian& angle);

		/**
		 * @brief	Rotates around Y axis.
		 *
		 * @param	angle	Angle to rotate by.
		 */
		void yaw(const Radian& angle);

		/**
		 * @brief	Rotates around X axis
		 *
		 * @param	angle	Angle to rotate by.
		 */
		void pitch(const Radian& angle);

		/**
		 * @brief	Forces any dirty transform matrices on this object to be updated.
		 *
		 * @note	Normally this is done internally when retrieving a transform, but sometimes
		 *			it is useful to update transforms manually.
		 */
		void updateTransformsIfDirty();

		/**
		 * @brief	Checks is the core dirty flag set. This is used by external systems 
		 *			to know when internal data has changed and core thread potentially needs to be notified.
		 */
		bool _isCoreDirty() const { return mIsCoreDirtyFlags != 0; }

		/**
		 * @brief	Marks the core dirty flag as clean.
		 */
		void _markCoreClean() { mIsCoreDirtyFlags = 0; }

	private:
		Vector3 mPosition;
		Quaternion mRotation;
		Vector3 mScale;

		mutable Vector3 mWorldPosition;
		mutable Quaternion mWorldRotation;
		mutable Vector3 mWorldScale;

		mutable Matrix4 mCachedLocalTfrm;
		mutable Matrix4 mCachedWorldTfrm;

		mutable UINT32 mDirtyFlags;
		mutable UINT32 mIsCoreDirtyFlags;

		/**
		 * @brief	Marks the transform as dirty so that we know to update
		 *			it when the transform is requested.
		 */
		void markTfrmDirty() const;

		/**
		 * @brief	Updates the local transform. Normally just reconstructs the 
		 *			transform matrix from the position/rotation/scale.
		 */
		void updateLocalTfrm() const;

		/**
		 * @brief	Updates the world transform. Reconstructs the local transform
		 *			matrix and multiplies it with any parent transforms.
		 *
		 * @note	If parent transforms are dirty they will be updated.
		 */
		void updateWorldTfrm() const;

		/**
		 * @brief	Checks if cached local transform needs updating.
		 */
		bool isCachedLocalTfrmUpToDate() const { return (mDirtyFlags & DirtyFlags::LocalTfrmDirty) == 0; }

		/**
		 * @brief	Checks if cached world transform needs updating.
		 */
		bool isCachedWorldTfrmUpToDate() const { return (mDirtyFlags & DirtyFlags::WorldTfrmDirty) == 0; }

		/************************************************************************/
		/* 								Hierarchy	                     		*/
		/************************************************************************/
	public:
		/**
		 * @brief	Changes the parent of this object. Also removes the object from the current parent,
		 * 			and assigns it to the new parent.
		 *
		 * @param [in]	parent	New parent.
		 */
		void setParent(const HSceneObject& parent);

		/**
		 * @brief	Gets the parent of this object.
		 *
		 * @return	Parent object, or nullptr if this SceneObject is at root level.
		 */
		HSceneObject getParent() const { return mParent; }

		/**
		 * @brief	Gets a child of this item.
		 *
		 * @param	idx	The zero based index of the child.
		 *
		 * @return	SceneObject of the child.
		 */
		HSceneObject getChild(UINT32 idx) const;

		/**
		 * @brief	Find the index of the specified child. Don't persist this value as
		 * 			it may change whenever you add/remove children.
		 *
		 * @param	child	The child to look for.
		 *
		 * @return	The zero-based index of the found child, or -1 if no match was found.
		 */
		int indexOfChild(const HSceneObject& child) const;

		/**
		 * @brief	Gets the number of all child GameObjects.
		 */
		UINT32 getNumChildren() const { return (UINT32)mChildren.size(); }

		/**
		 * @brief	Enables or disables this object. Disabled objects also implicitly disable
		 *			all their child objects. No components on the disabled object are updated.
		 */
		void setActive(bool active);

		/**
		 * @brief	Returns whether or not an object is active.
		 *
		 * @param	self	If true, the method will only check if this particular object was activated
		 *					or deactivated directly via setActive. If false we we also check if any of
		 *					the objects parents are inactive.
		 */
		bool getActive(bool self = false);

		/**
		 * @brief	Makes a deep copy of this object.
		 */
		HSceneObject clone();

	private:
		HSceneObject mParent;
		Vector<HSceneObject> mChildren;
		bool mActiveSelf;
		bool mActiveHierarchy;

		/**
		 * @brief	Adds a child to the child array. This method doesn't check for null or duplicate values.
		 *
		 * @param [in]	object	New child.
		 */
		void addChild(const HSceneObject& object);
		
		/**
		 * @brief	Removes the child from the object. 
		 *
		 * @param [in]	object	Child to remove.
		 * 					
		 * @throws INTERNAL_ERROR If the provided child isn't a child of the current object.
		 */
		void removeChild(const HSceneObject& object);

		/**
		 * @brief	Changes the object active in hierarchy state.
		 */
		void setActiveHierarchy(bool active);

		/************************************************************************/
		/* 								Component	                     		*/
		/************************************************************************/
	public:
		/**
		 * @brief	Constructs a new component of the specified type and adds it to 
		 *			the internal component list.
		 */
		template<class T, class... Args>
		GameObjectHandle<T> addComponent(Args &&... args)
		{
			static_assert((std::is_base_of<BansheeEngine::Component, T>::value),
				"Specified type is not a valid Component.");

			std::shared_ptr<T> gameObject(new (bs_alloc<T, PoolAlloc>()) T(mThisHandle,
				std::forward<Args>(args)...),
				&bs_delete<PoolAlloc, T>, StdAlloc<T, PoolAlloc>());

			GameObjectHandle<T> newComponent =
				GameObjectHandle<T>(GameObjectManager::instance().registerObject(gameObject));

			mComponents.push_back(newComponent);

			gCoreSceneManager().notifyComponentAdded(newComponent);	

			return newComponent;
		}

		/**
		 * @brief	Searches for a component with the specific type and returns the first one
		 * 			it finds. 
		 * 			
		 * @note	Don't call this too often as it is relatively slow. It is more efficient 
		 * 			to call it once and store the result for further use.
		 *
		 * @tparam	typename T	Type of the component.
		 *
		 * @return	Component if found, nullptr otherwise.
		 */
		template <typename T>
		GameObjectHandle<T> getComponent()
		{
			static_assert((std::is_base_of<BansheeEngine::Component, T>::value), 
				"Specified type is not a valid Component.");

			return static_object_cast<T>(getComponent(T::getRTTIStatic()->getRTTIId()));
		}

		/**
		 * @brief	Checks if the current object contains the specified component
		 * 			 			
		 * @note	Don't call this too often as it is relatively slow. 
		 *
		 * @tparam	typename T	Type of the component.
		 *
		 * @return	True if component exists on the object.
		 */
		template <typename T>
		bool hasComponent()
		{
			static_assert((std::is_base_of<BansheeEngine::Component, T>::value), 
				"Specified type is not a valid Component.");

			for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
			{
				if ((*iter)->getRTTI()->getRTTIId() == T::getRTTIStatic()->getRTTIId())
					return true;
			}

			return false;
		}

		/**
		 * @brief	Searches for a component with the specified type id and returns the first one it
		 * 			finds.
		 * 			
		 * 			@note	Don't call this too often as it is relatively slow. It is more efficient to
		 * 			call it once and store the result for further use.
		 *
		 * @param	typeId	Identifier for the type.
		 *
		 * @return	Component if found, nullptr otherwise.
		 */
		HComponent getComponent(UINT32 typeId) const;

		/**
		 * @brief	Removes the component from this object, and deallocates it.
		 *
		 * @param [in]	component	The component to destroy.
		 */
		void destroyComponent(const HComponent& component);

		/**
		 * @brief	Removes the component from this object, and deallocates it.
		 *
		 * @param [in]	component	The component to destroy.
		 */
		void destroyComponent(Component* component);

		/**
		 * @brief	Returns all components on this object.
		 */
		const Vector<HComponent>& getComponents() const { return mComponents; }

	private:
		/**
		 * @brief	Creates an empty component with the default constructor.
		 */
		template <typename T>
		static std::shared_ptr<T> createEmptyComponent()
		{
			static_assert((std::is_base_of<BansheeEngine::Component, T>::value), "Specified type is not a valid Component.");

			std::shared_ptr<T> gameObject(new (bs_alloc<T, PoolAlloc>()) T(), &bs_delete<PoolAlloc, T>, StdAlloc<T, PoolAlloc>());
			GameObjectHandle<T>(GameObjectManager::instance().registerObject(gameObject));

			return gameObject;
		}

		/**
		 * @brief	Adds the component to the internal component array.
		 */
		void addComponentInternal(const std::shared_ptr<Component> component);

		Vector<HComponent> mComponents;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class GameObjectRTTI;
		friend class SceneObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}