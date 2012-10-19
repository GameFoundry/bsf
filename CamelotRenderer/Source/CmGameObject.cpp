#include "CmGameObject.h"
#include "CmComponent.h"
#include "CmSceneManager.h"
#include "CmException.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	GameObject::GameObject(const String& name)
		:mName(name), mPosition(Vector3::ZERO), mRotation(Quaternion::IDENTITY), mScale(Vector3::UNIT_SCALE),
		mWorldPosition(Vector3::ZERO), mWorldRotation(Quaternion::IDENTITY), mWorldScale(Vector3::UNIT_SCALE),
		mCachedLocalTfrm(Matrix4::IDENTITY), mIsCachedLocalTfrmUpToDate(false),
		mCachedWorldTfrm(Matrix4::IDENTITY), mIsCachedWorldTfrmUpToDate(false),
		mCustomWorldTfrm(Matrix4::IDENTITY), mIsCustomTfrmModeActive(false),
		mIsDestroyed(false)
	{ }

	GameObject::~GameObject()
	{
		if(!mIsDestroyed)
			destroy();
	}

	GameObjectPtr GameObject::create(const String& name)
	{
		GameObjectPtr newObject = createInternal(name);

		gSceneManager().registerNewGO(newObject);

		return newObject;
	}

	GameObjectPtr GameObject::createInternal(const String& name)
	{
		GameObjectPtr newObject = GameObjectPtr(new GameObject(name));
		newObject->mThis = newObject;

		return newObject;
	}

	void GameObject::destroy()
	{
		mIsDestroyed = true;

		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
			(*iter)->destroy();

		mChildren.clear();

		for(auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
			(*iter)->destroy();

		mComponents.clear();

		// Parent is our owner, so when his reference to us is removed, delete might be called.
		// So make sure this is the last thing we do.
		if(!mParent.expired())
		{
			GameObjectPtr parentPtr = mParent.lock();

			if(!parentPtr->isDestroyed())
				parentPtr->removeChild(mThis.lock());
		}
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

	const Vector3& GameObject::getWorldPosition() const
	{ 
		if(!mIsCachedWorldTfrmUpToDate)
			updateWorldTfrm();

		return mWorldPosition; 
	}

	const Quaternion& GameObject::getWorldRotation() const 
	{ 
		if(!mIsCachedWorldTfrmUpToDate)
			updateWorldTfrm();

		return mWorldRotation; 
	}

	const Vector3& GameObject::getWorldScale() const 
	{ 
		if(!mIsCachedWorldTfrmUpToDate)
			updateWorldTfrm();

		return mWorldScale; 
	}

	void GameObject::lookAt(const Vector3& location, const Vector3& up)
	{
		Vector3 forward = location - mPosition;
		forward.normalise();

		Vector3 upCopy = up;
		upCopy.normalise();

		Vector3 right = forward.crossProduct(up);
		right.normalise();

		Quaternion newRotation;
		newRotation.FromAxes(right, upCopy, forward);

		setRotation(newRotation);
	}

	const Matrix4& GameObject::getWorldTfrm() const
	{
		if(!mIsCachedWorldTfrmUpToDate)
			updateWorldTfrm();

		return mCachedWorldTfrm;
	}

	const Matrix4& GameObject::getLocalTfrm() const
	{
		if(!mIsCachedLocalTfrmUpToDate)
			updateLocalTfrm();

		return mCachedLocalTfrm;
	}

	void GameObject::markTfrmDirty() const
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

	void GameObject::updateWorldTfrm() const
	{
		if(!mParent.expired())
		{
			GameObjectPtr tempParentPtr = mParent.lock();

			mCachedWorldTfrm = getLocalTfrm() * tempParentPtr->getWorldTfrm();

			// Update orientation
			const Quaternion& parentOrientation = tempParentPtr->getWorldRotation();
			mWorldRotation = parentOrientation * mRotation;

			// Update scale
			const Vector3& parentScale = tempParentPtr->getWorldScale();
			// Scale own position by parent scale, NB just combine
			// as equivalent axes, no shearing
			mWorldScale = parentScale * mScale;

			// Change position vector based on parent's orientation & scale
			mWorldPosition = parentOrientation * (parentScale * mPosition);

			// Add altered position vector to parents
			mWorldPosition += tempParentPtr->getWorldPosition();
		}
		else
		{
			mCachedWorldTfrm = getLocalTfrm();

			mWorldRotation = mRotation;
			mWorldPosition = mPosition;
			mWorldScale = mScale;
		}

		mIsCachedWorldTfrmUpToDate = true;
	}

	void GameObject::updateLocalTfrm() const
	{
		mCachedLocalTfrm.makeTransform(mPosition, mScale, mRotation);

		mIsCachedLocalTfrmUpToDate = true;
	}

	/************************************************************************/
	/* 								Hierarchy	                     		*/
	/************************************************************************/

	void GameObject::setParent(GameObjectPtr parent)
	{
		if(parent == nullptr || parent->isDestroyed())
		{
			CM_EXCEPT(InternalErrorException, 
				"Parent is not allowed to be NULL or destroyed.");
		}

		if(mParent.expired() || mParent.lock() != parent)
		{
			if(!mParent.expired())
				mParent.lock()->removeChild(mThis.lock());

			if(parent != nullptr)
				parent->addChild(mThis.lock());

			mParent = parent;
			markTfrmDirty();
		}
	}

	GameObjectPtr GameObject::getChild(unsigned int idx) const
	{
		if(idx < 0 || idx >= mChildren.size())
		{
			CM_EXCEPT(InternalErrorException, 
				"Child index out of range.");
		}

		return mChildren[idx];
	}

	int GameObject::indexOfChild(const GameObjectPtr child) const
	{
		for(int i = 0; i < (int)mChildren.size(); i++)
		{
			if(mChildren[i] == child)
				return i;
		}

		return -1;
	}

	void GameObject::addChild(GameObjectPtr object)
	{
		mChildren.push_back(object); 
	}

	void GameObject::removeChild(GameObjectPtr object)
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

	void GameObject::destroyComponent(ComponentPtr component)
	{
		if(component == nullptr)
		{
			LOGDBG("Trying to remove a null component");
			return;
		}

		auto iter = std::find(mComponents.begin(), mComponents.end(), component);

		if(iter != mComponents.end())
		{
			(*iter)->destroy();
			mComponents.erase(iter);

			gSceneManager().notifyComponentRemoved((*iter));
		}
		else
			LOGDBG("Trying to remove a component that doesn't exist on this GameObject.");
	}
}