#pragma once

#include "CmPrerequisites.h"
#include "CmMatrix4.h"
#include "CmVector3.h"
#include "CmQuaternion.h"

#include "boost/static_assert.hpp"

namespace CamelotEngine
{
	class CM_EXPORT GameObject
	{
		friend class SceneManager;
	public:
		static GameObjectPtr create(const String& name);
		~GameObject();

		void destroy();
		bool isDestroyed() { return mIsDestroyed; }

	private:
		GameObject(const String& name);
		static GameObjectPtr createInternal(const String& name);

		std::weak_ptr<GameObject> mThis;
		bool mIsDestroyed;

		/************************************************************************/
		/* 								Transform	                     		*/
		/************************************************************************/
	public:
		void setPosition(const Vector3& position);
		const Vector3& getPosition() const { return mPosition; }
		const Vector3& getWorldPosition() const;

		void setRotation(const Quaternion& rotation);
		const Quaternion& getRotation() const { return mRotation; }
		const Quaternion& getWorldRotation() const;

		void setScale(const Vector3& scale);
		const Vector3& getScale() const { return mScale; }
		const Vector3& getWorldScale() const;

		void lookAt(const Vector3& location, const Vector3& up = Vector3::UNIT_Y);

		const Matrix4& getWorldTfrm() const;
		const Matrix4& getLocalTfrm() const;

	private:
		String mName;

		Vector3 mPosition;
		Quaternion mRotation;
		Vector3 mScale;

		mutable Vector3 mWorldPosition;
		mutable Quaternion mWorldRotation;
		mutable Vector3 mWorldScale;

		mutable Matrix4 mCachedLocalTfrm;
		mutable bool mIsCachedLocalTfrmUpToDate;

		mutable Matrix4 mCachedWorldTfrm;
		mutable bool mIsCachedWorldTfrmUpToDate;

		Matrix4 mCustomWorldTfrm; // TODO
		bool mIsCustomTfrmModeActive; // TODO

		void markTfrmDirty() const;
		void updateLocalTfrm() const;
		void updateWorldTfrm() const;

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
		void setParent(GameObjectPtr parent);

		/**
		 * @brief	Gets the parent of this object.
		 *
		 * @return	Parent object, or nullptr if this GameObject is at root level.
		 */
		GameObjectPtr getParent() const { return mParent.lock(); }

		/**
		 * @brief	Gets a child of this item.
		 *
		 * @param	idx	The zero based index of the child.
		 *
		 * @return	GameObject of the child.
		 * 			
		 * @throws ERR_INVALIDPARAMS If the index is out of range.
		 */
		GameObjectPtr getChild(unsigned int idx) const;

		/**
		 * @brief	Find the index of the specified child. Don't persist this value as
		 * 			it may change whenever you add/remove children.
		 *
		 * @param	child	The child to look for.
		 *
		 * @return	The zero-based index of the found child, or -1 if no match was found.
		 */
		int indexOfChild(const GameObjectPtr child) const;

		/**
		 * @brief	Gets the number of all child GameObjects.
		 */
		int getNumChildren() const { return mChildren.size(); }

	private:
		std::weak_ptr<GameObject> mParent;
		vector<GameObjectPtr>::type mChildren;

		/**
		 * @brief	Adds a child to the child array. This method doesn't check for null or duplicate values.
		 *
		 * @param [in]	object	New child.
		 */
		void addChild(GameObjectPtr object);
		
		/**
		 * @brief	Removes the child from the object. 
		 *
		 * @param [in]	object	Child to remove.
		 * 					
		 * @throws INTERNAL_ERROR If the provided child isn't a child of the current object.
		 */
		void removeChild(GameObjectPtr object);

		/************************************************************************/
		/* 								Component	                     		*/
		/************************************************************************/
	public:
		template <typename T>
		std::shared_ptr<T> addComponent()
		{
			BOOST_STATIC_ASSERT_MSG((boost::is_base_of<CamelotEngine::Component, T>::value), 
				"Specified type is not a valid Component.");

			std::shared_ptr<T> newComponent = std::shared_ptr<T>(new T(mThis.lock()));
			mComponents.push_back(newComponent);

			return newComponent;
		}

		/**
		 * @brief	Removes the component from this GameObject, and deallocates it.
		 *
		 * @param [in]	component	The component to destroy.
		 */
		void destroyComponent(ComponentPtr component);

	private:
		vector<ComponentPtr>::type mComponents;
	};
}