#pragma once

#include "CmPrerequisites.h"
#include "CmMatrix4.h"
#include "CmVector3.h"
#include "CmQuaternion.h"

namespace CamelotEngine
{
	class CM_EXPORT GameObject
	{
	public:
		GameObject(const String& name);
		~GameObject();

		/************************************************************************/
		/* 								Transform	                     		*/
		/************************************************************************/
	public:
		void setPosition(const Vector3& position);
		Vector3 getPosition() const { return mPosition; }

		void setRotation(const Quaternion& rotation);
		Quaternion getRotation() const { return mRotation; }

		void setScale(const Vector3& scale);
		Vector3 getScale() const { return mScale; }

		const Matrix4& getWorldTfrm();
		const Matrix4& getLocalTfrm();

	private:
		String mName;

		Vector3 mPosition;
		Quaternion mRotation;
		Vector3 mScale;

		Matrix4 mCachedLocalTfrm;
		bool mIsCachedLocalTfrmUpToDate;

		Matrix4 mCachedWorldTfrm;
		bool mIsCachedWorldTfrmUpToDate;

		Matrix4 mCustomWorldTfrm; // TODO
		bool mIsCustomTfrmModeActive; // TODO

		void markTfrmDirty();
		void updateLocalTfrm();
		void updateWorldTfrm();

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
		void setParent(GameObject* parent);

		/**
		 * @brief	Gets the parent of this object.
		 *
		 * @return	Parent object, or nullptr if this GameObject is at root level.
		 */
		GameObject* getParent() const { return mParent; }

		/**
		 * @brief	Gets a child of this item.
		 *
		 * @param	idx	The zero based index of the child.
		 *
		 * @return	GameObject of the child.
		 * 			
		 * @throws ERR_INVALIDPARAMS If the index is out of range.
		 */
		GameObject* getChild(unsigned int idx) const;

		/**
		 * @brief	Find the index of the specified child. Don't persist this value as
		 * 			it may change whenever you add/remove children.
		 *
		 * @param	child	The child to look for.
		 *
		 * @return	The zero-based index of the found child, or -1 if no match was found.
		 */
		int indexOfChild(const GameObject* child) const;

		/**
		 * @brief	Gets the number of all child GameObjects.
		 */
		int getNumChildren() const { return mChildren.size(); }

	private:
		GameObject* mParent;
		vector<GameObject*>::type mChildren;

		/**
		 * @brief	Adds a child to the child array. This method doesn't check for null or duplicate values.
		 *
		 * @param [in]	object	New child.
		 */
		void addChild(GameObject* object);
		
		/**
		 * @brief	Removes the child from the object. 
		 *
		 * @param [in]	object	Child to remove.
		 * 					
		 * @throws INTERNAL_ERROR If the provided child isn't a child of the current object.
		 */
		void removeChild(GameObject* object);
	};
}