#include "CmSceneObject.h"
#include "CmComponent.h"
#include "CmSceneManager.h"
#include "CmException.h"
#include "CmDebug.h"
#include "CmSceneObjectRTTI.h"

namespace CamelotFramework
{
	SceneObject::SceneObject(const String& name)
		:mName(name), mPosition(Vector3::ZERO), mRotation(Quaternion::IDENTITY), mScale(Vector3::ONE),
		mWorldPosition(Vector3::ZERO), mWorldRotation(Quaternion::IDENTITY), mWorldScale(Vector3::ONE),
		mCachedLocalTfrm(Matrix4::IDENTITY), mIsCachedLocalTfrmUpToDate(false),
		mCachedWorldTfrm(Matrix4::IDENTITY), mIsCachedWorldTfrmUpToDate(false),
		mCustomWorldTfrm(Matrix4::IDENTITY), mIsCustomTfrmModeActive(false)
	{ }

	SceneObject::~SceneObject()
	{
		if(!mThisHandle.isDestroyed())
		{
			LOGWRN("Object is being deleted without being destroyed first?");
			destroyInternal();
		}
	}

	HSceneObject SceneObject::create(const String& name)
	{
		HSceneObject newObject = createInternal(name);

		gSceneManager().registerNewGO(newObject);

		return newObject;
	}

	HSceneObject SceneObject::createInternal(const String& name)
	{
		HSceneObject sceneObject = GameObjectHandle<SceneObject>(
			new (cm_alloc<SceneObject, PoolAlloc>()) SceneObject(name),
			&cm_delete<PoolAlloc, GameObject>);
		sceneObject->mThisHandle = sceneObject;

		return sceneObject;
	}

	void SceneObject::destroy()
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

	void SceneObject::destroyInternal()
	{
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
			(*iter)->destroyInternal();

		mChildren.clear();

		for(auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
		{
			gSceneManager().notifyComponentRemoved((*iter));
			(*iter).destroy();
		}

		mComponents.clear();

		mThisHandle.destroy();
	}

	/************************************************************************/
	/* 								Transform	                     		*/
	/************************************************************************/

	void SceneObject::setPosition(const Vector3& position)
	{
		mPosition = position;
		markTfrmDirty();
	}

	void SceneObject::setRotation(const Quaternion& rotation)
	{
		mRotation = rotation;
		markTfrmDirty();
	}

	void SceneObject::setScale(const Vector3& scale)
	{
		mScale = scale;
		markTfrmDirty();
	}

	const Vector3& SceneObject::getWorldPosition() const
	{ 
		if(!mIsCachedWorldTfrmUpToDate)
			updateWorldTfrm();

		return mWorldPosition; 
	}

	const Quaternion& SceneObject::getWorldRotation() const 
	{ 
		if(!mIsCachedWorldTfrmUpToDate)
			updateWorldTfrm();

		return mWorldRotation; 
	}

	const Vector3& SceneObject::getWorldScale() const 
	{ 
		if(!mIsCachedWorldTfrmUpToDate)
			updateWorldTfrm();

		return mWorldScale; 
	}

	void SceneObject::lookAt(const Vector3& location, const Vector3& up)
	{
		Vector3 forward = location - mPosition;
		forward.normalize();

		// TODO - I'm ignoring "up" direction
		setForward(forward);

		Quaternion upRot = getUp().getRotationTo(up);
		setRotation(getRotation() * upRot);
	}

	const Matrix4& SceneObject::getWorldTfrm() const
	{
		if(!mIsCachedWorldTfrmUpToDate)
			updateWorldTfrm();

		return mCachedWorldTfrm;
	}

	const Matrix4& SceneObject::getLocalTfrm() const
	{
		if(!mIsCachedLocalTfrmUpToDate)
			updateLocalTfrm();

		return mCachedLocalTfrm;
	}

	void SceneObject::move(const Vector3& vec)
	{
		setPosition(mPosition + vec);
	}

	void SceneObject::moveRelative(const Vector3& vec)
	{
		// Transform the axes of the relative vector by camera's local axes
		Vector3 trans = mRotation * vec;

		setPosition(mPosition + trans);
	}

	void SceneObject::rotate(const Vector3& axis, const Radian& angle)
	{
		Quaternion q;
		q.FromAngleAxis(angle,axis);
		rotate(q);
	}

	void SceneObject::rotate(const Quaternion& q)
	{
		// Note the order of the mult, i.e. q comes after

		// Normalize the quat to avoid cumulative problems with precision
		Quaternion qnorm = q;
		qnorm.normalize();
		setRotation(qnorm * mRotation);
	}

	void SceneObject::roll(const Radian& angle)
	{
		// Rotate around local Z axis
		Vector3 zAxis = mRotation * Vector3::UNIT_Z;
		rotate(zAxis, angle);
	}

	void SceneObject::yaw(const Radian& angle)
	{
		Vector3 yAxis = mRotation * Vector3::UNIT_Y;
		rotate(yAxis, angle);
	}

	void SceneObject::pitch(const Radian& angle)
	{
		// Rotate around local X axis
		Vector3 xAxis = mRotation * Vector3::UNIT_X;
		rotate(xAxis, angle);
	}

	void SceneObject::setForward(const Vector3& forwardDir)
	{
		if (forwardDir == Vector3::ZERO) 
			return;

		Vector3 nrmForwardDir = Vector3::normalize(forwardDir);
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

	void SceneObject::markTfrmDirty() const
	{
		mIsCachedLocalTfrmUpToDate = false;
		mIsCachedWorldTfrmUpToDate = false;

		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			(*iter)->markTfrmDirty();
		}
	}

	void SceneObject::updateWorldTfrm() const
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

	void SceneObject::updateLocalTfrm() const
	{
		mCachedLocalTfrm.makeTransform(mPosition, mScale, mRotation);

		mIsCachedLocalTfrmUpToDate = true;
	}

	/************************************************************************/
	/* 								Hierarchy	                     		*/
	/************************************************************************/

	void SceneObject::setParent(const HSceneObject& parent)
	{
		if(parent.isDestroyed())
		{
			CM_EXCEPT(InternalErrorException, 
				"Trying to assign a SceneObject parent that is destroyed.");
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

	HSceneObject SceneObject::getChild(unsigned int idx) const
	{
		if(idx < 0 || idx >= mChildren.size())
		{
			CM_EXCEPT(InternalErrorException, 
				"Child index out of range.");
		}

		return mChildren[idx];
	}

	int SceneObject::indexOfChild(const HSceneObject& child) const
	{
		for(int i = 0; i < (int)mChildren.size(); i++)
		{
			if(mChildren[i] == child)
				return i;
		}

		return -1;
	}

	void SceneObject::addChild(const HSceneObject& object)
	{
		mChildren.push_back(object); 
	}

	void SceneObject::removeChild(const HSceneObject& object)
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

	HComponent SceneObject::getComponent(UINT32 typeId) const
	{
		for(auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
		{
			if((*iter)->getRTTI()->getRTTIId() == typeId)
				return *iter;
		}

		return HComponent();
	}

	void SceneObject::destroyComponent(const HComponent& component)
	{
		if(component == nullptr)
		{
			LOGDBG("Trying to remove a null component");
			return;
		}

		auto iter = std::find(mComponents.begin(), mComponents.end(), component);

		if(iter != mComponents.end())
		{
			gSceneManager().notifyComponentRemoved((*iter));

			(*iter).destroy();
			mComponents.erase(iter);
		}
		else
			LOGDBG("Trying to remove a component that doesn't exist on this SceneObject.");
	}

	void SceneObject::destroyComponent(Component* component)
	{
		auto iterFind = std::find_if(mComponents.begin(), mComponents.end(), 
			[component](const HComponent& x) { return x.getHandleData()->mPtr == component; });

		if(iterFind == mComponents.end())
		{
			LOGDBG("Trying to remove a component that doesn't exist on this SceneObject.");
		}
		else
		{
			destroyComponent(*iterFind);
		}
	}

	RTTITypeBase* SceneObject::getRTTIStatic()
	{
		return SceneObjectRTTI::instance();
	}

	RTTITypeBase* SceneObject::getRTTI() const
	{
		return SceneObject::getRTTIStatic();
	}
}