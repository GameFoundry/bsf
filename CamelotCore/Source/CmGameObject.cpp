#include "CmGameObject.h"
#include "CmComponent.h"
#include "CmSceneManager.h"
#include "CmException.h"
#include "CmDebug.h"
#include "CmGameObjectRTTI.h"

namespace CamelotEngine
{
	GameObject::GameObject(const String& name)
		:mName(name), mPosition(Vector3::ZERO), mRotation(Quaternion::IDENTITY), mScale(Vector3::UNIT_SCALE),
		mWorldPosition(Vector3::ZERO), mWorldRotation(Quaternion::IDENTITY), mWorldScale(Vector3::UNIT_SCALE),
		mCachedLocalTfrm(Matrix4::IDENTITY), mIsCachedLocalTfrmUpToDate(false),
		mCachedWorldTfrm(Matrix4::IDENTITY), mIsCachedWorldTfrmUpToDate(false),
		mCustomWorldTfrm(Matrix4::IDENTITY), mIsCustomTfrmModeActive(false)
	{ }

	GameObject::~GameObject()
	{
		if(!mThisHandle.isDestroyed())
		{
			LOGWRN("Object is being deleted without being destroyed first?");
			destroyInternal();
		}
	}

	HGameObject GameObject::create(const String& name)
	{
		HGameObject newObject = createInternal(name);

		gSceneManager().registerNewGO(newObject);

		return newObject;
	}

	HGameObject GameObject::createInternal(const String& name)
	{
		HGameObject gameObject = HGameObject::_create(new GameObject(name));
		gameObject->mThisHandle = gameObject;

		return gameObject;
	}

	void GameObject::destroy()
	{
		// Parent is our owner, so when his reference to us is removed, delete might be called.
		// So make sure this is the last thing we do.
		if(mParent != nullptr)
		{
			if(!mParent.isDestroyed())
				mParent->removeChild(mThisHandle);
		}

		destroyInternal();
	}

	void GameObject::destroyInternal()
	{
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
			(*iter)->destroyInternal();

		mChildren.clear();

		for(auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
		{
			(*iter).destroy();
		}

		mComponents.clear();

		mThisHandle.destroy();
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

		// TODO - I'm ignoring "up" direction
		setForward(forward);

		Quaternion upRot = getUp().getRotationTo(up);
		setRotation(getRotation() * upRot);
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

	void GameObject::move(const Vector3& vec)
	{
		setPosition(mPosition + vec);
	}

	void GameObject::moveRelative(const Vector3& vec)
	{
		// Transform the axes of the relative vector by camera's local axes
		Vector3 trans = mRotation * vec;

		setPosition(mPosition + trans);
	}

	void GameObject::rotate(const Vector3& axis, const Radian& angle)
	{
		Quaternion q;
		q.FromAngleAxis(angle,axis);
		rotate(q);
	}

	void GameObject::rotate(const Quaternion& q)
	{
		// Note the order of the mult, i.e. q comes after

		// Normalize the quat to avoid cumulative problems with precision
		Quaternion qnorm = q;
		qnorm.normalise();
		setRotation(qnorm * mRotation);
	}

	void GameObject::roll(const Radian& angle)
	{
		// Rotate around local Z axis
		Vector3 zAxis = mRotation * Vector3::UNIT_Z;
		rotate(zAxis, angle);
	}

	void GameObject::yaw(const Radian& angle)
	{
		Vector3 yAxis = mRotation * Vector3::UNIT_Y;
		rotate(yAxis, angle);
	}

	void GameObject::pitch(const Radian& angle)
	{
		// Rotate around local X axis
		Vector3 xAxis = mRotation * Vector3::UNIT_X;
		rotate(xAxis, angle);
	}

	void GameObject::setForward(const Vector3& forwardDir)
	{
		if (forwardDir == Vector3::ZERO) 
			return;

		Vector3 nrmForwardDir = forwardDir.normalisedCopy();
		Vector3 currentForwardDir = getForward();
		
		const Quaternion& currentRotation = getWorldRotation();
		Quaternion targetRotation;
		if ((nrmForwardDir+currentForwardDir).squaredLength() < 0.00005f)
		{
			// Oops, a 180 degree turn (infinite possible rotation axes)
			// Default to yaw i.e. use current UP
			targetRotation = Quaternion(-currentRotation.y, -currentRotation.z, currentRotation.w, currentRotation.x);
		}
		else
		{
			// Derive shortest arc to new direction
			Quaternion rotQuat = currentForwardDir.getRotationTo(nrmForwardDir);
			targetRotation = rotQuat * currentRotation;
		}

		setRotation(targetRotation);
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
		if(mParent != nullptr)
		{
			mCachedWorldTfrm = getLocalTfrm() * mParent->getWorldTfrm();

			// Update orientation
			const Quaternion& parentOrientation = mParent->getWorldRotation();
			mWorldRotation = parentOrientation * mRotation;

			// Update scale
			const Vector3& parentScale = mParent->getWorldScale();
			// Scale own position by parent scale, NB just combine
			// as equivalent axes, no shearing
			mWorldScale = parentScale * mScale;

			// Change position vector based on parent's orientation & scale
			mWorldPosition = parentOrientation * (parentScale * mPosition);

			// Add altered position vector to parents
			mWorldPosition += mParent->getWorldPosition();
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

	void GameObject::setParent(const HGameObject& parent)
	{
		if(parent.isDestroyed())
		{
			CM_EXCEPT(InternalErrorException, 
				"Trying to assign a GameObject parent that is destroyed.");
		}

		if(mParent == nullptr || mParent != parent)
		{
			if(mParent != nullptr)
				mParent->removeChild(mThisHandle);

			if(parent != nullptr)
				parent->addChild(mThisHandle);

			mParent = parent;
			markTfrmDirty();
		}
	}

	HGameObject GameObject::getChild(unsigned int idx) const
	{
		if(idx < 0 || idx >= mChildren.size())
		{
			CM_EXCEPT(InternalErrorException, 
				"Child index out of range.");
		}

		return mChildren[idx];
	}

	int GameObject::indexOfChild(const HGameObject& child) const
	{
		for(int i = 0; i < (int)mChildren.size(); i++)
		{
			if(mChildren[i] == child)
				return i;
		}

		return -1;
	}

	void GameObject::addChild(const HGameObject& object)
	{
		mChildren.push_back(object); 
	}

	void GameObject::removeChild(const HGameObject& object)
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

	HComponent GameObject::getComponent(UINT32 typeId) const
	{
		for(auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
		{
			if((*iter)->getRTTI()->getRTTIId() == typeId)
				return *iter;
		}

		return HComponent();
	}

	void GameObject::destroyComponent(const HComponent& component)
	{
		if(component == nullptr)
		{
			LOGDBG("Trying to remove a null component");
			return;
		}

		auto iter = std::find(mComponents.begin(), mComponents.end(), component);

		if(iter != mComponents.end())
		{
			(*iter).destroy();
			mComponents.erase(iter);

			gSceneManager().notifyComponentRemoved((*iter));
		}
		else
			LOGDBG("Trying to remove a component that doesn't exist on this GameObject.");
	}

	RTTITypeBase* GameObject::getRTTIStatic()
	{
		return GameObjectRTTI::instance();
	}

	RTTITypeBase* GameObject::getRTTI() const
	{
		return GameObject::getRTTIStatic();
	}
}