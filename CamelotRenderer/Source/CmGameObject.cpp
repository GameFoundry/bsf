#include "CmGameObject.h"
#include "CmException.h"

namespace CamelotEngine
{
	GameObject::GameObject(const String& name)
		:mName(name), mPosition(Vector3::ZERO), mRotation(Quaternion::IDENTITY), mScale(Vector3::ZERO),
		mCachedLocalTfrm(Matrix4::IDENTITY), mIsCachedLocalTfrmUpToDate(false),
		mCachedWorldTfrm(Matrix4::IDENTITY), mIsCachedWorldTfrmUpToDate(false),
		mCustomWorldTfrm(Matrix4::IDENTITY), mIsCustomTfrmModeActive(false),
		mParent(nullptr)
	{ }

	GameObject::~GameObject()
	{
		if(mParent != nullptr)
			mParent->removeChild(this);
	}

	/************************************************************************/
	/* 								Transform	                     		*/
	/************************************************************************/

	void GameObject::setPosition(const Vector3& position)
	{
		mPosition = position;
		markTfrmDirty();
	}

	void GameObject::setRotation(const Quaternion& rotation)
	{
		mRotation = rotation;
		markTfrmDirty();
	}

	void GameObject::setScale(const Vector3& scale)
	{
		mScale = scale;
		markTfrmDirty();
	}

	const Matrix4& GameObject::getWorldTfrm()
	{
		if(!mIsCachedWorldTfrmUpToDate)
			updateWorldTfrm();

		return mCachedWorldTfrm;
	}

	const Matrix4& GameObject::getLocalTfrm()
	{
		if(!mIsCachedLocalTfrmUpToDate) // TODO - Low priority - This unnecessarily updates world transform as well
			updateLocalTfrm();

		return mCachedLocalTfrm;
	}

	void GameObject::markTfrmDirty()
	{
		mIsCachedLocalTfrmUpToDate = false;

		if(mIsCachedWorldTfrmUpToDate) // If it's already marked as dirty, no point is recursing the hierarchy again
		{
			mIsCachedWorldTfrmUpToDate = false;

			for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
			{
				(*iter)->markTfrmDirty();
			}
		}
	}

	void GameObject::updateWorldTfrm()
	{
		if(mParent != nullptr)
			mCachedWorldTfrm = getLocalTfrm() * mParent->getWorldTfrm();
		else
			mCachedWorldTfrm = getLocalTfrm();

		mIsCachedWorldTfrmUpToDate = true;
	}

	void GameObject::updateLocalTfrm()
	{
		mCachedLocalTfrm.makeTransform(mPosition, mScale, mRotation);

		mIsCachedLocalTfrmUpToDate = true;
	}

	/************************************************************************/
	/* 								Hierarchy	                     		*/
	/************************************************************************/

	void GameObject::setParent(GameObject* parent)
	{
		if(parent == nullptr)
		{
			CM_EXCEPT(InternalErrorException, 
				"Parent is not allowed to be NULL.");
		}

		if(mParent != parent)
		{
			if(mParent != nullptr)
				mParent->removeChild(this);

			if(parent != nullptr)
				parent->addChild(this);

			mParent = parent;
			markTfrmDirty();
		}
	}

	GameObject* GameObject::getChild(unsigned int idx) const
	{
		if(idx < 0 || idx >= mChildren.size())
		{
			CM_EXCEPT(InternalErrorException, 
				"Child index out of range.");
		}

		return mChildren[idx];
	}

	int GameObject::indexOfChild(const GameObject* child) const
	{
		for(int i = 0; i < (int)mChildren.size(); i++)
		{
			if(mChildren[i] == child)
				return i;
		}

		return -1;
	}

	void GameObject::addChild(GameObject* object)
	{
		mChildren.push_back(object);
	}

	void GameObject::removeChild(GameObject* object)
	{
		auto result = find(mChildren.begin(), mChildren.end(), object);

		if(result != mChildren.end())
			mChildren.erase(result);
		else
		{
			CM_EXCEPT(InternalErrorException, 
				"Trying to remove a child but it's not a child of the transform.");
		}
	}
}