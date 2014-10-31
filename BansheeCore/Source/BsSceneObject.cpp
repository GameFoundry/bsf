#include "BsSceneObject.h"
#include "BsComponent.h"
#include "BsCoreSceneManager.h"
#include "BsException.h"
#include "BsDebug.h"
#include "BsSceneObjectRTTI.h"
#include "BsMemorySerializer.h"
#include "BsGameObjectManager.h"

namespace BansheeEngine
{
	SceneObject::SceneObject(const String& name)
		:GameObject(), mPosition(Vector3::ZERO), mRotation(Quaternion::IDENTITY), mScale(Vector3::ONE),
		mWorldPosition(Vector3::ZERO), mWorldRotation(Quaternion::IDENTITY), mWorldScale(Vector3::ONE),
		mCachedLocalTfrm(Matrix4::IDENTITY), mDirtyFlags(0xFFFFFFFF), mCachedWorldTfrm(Matrix4::IDENTITY), 
		mIsCoreDirtyFlags(0xFFFFFFFF), mActiveSelf(true), mActiveHierarchy(true), mDirtyHash(0)
	{
		setName(name);
	}

	SceneObject::~SceneObject()
	{
		if(!mThisHandle.isDestroyed())
		{
			LOGWRN("Object is being deleted without being destroyed first?");
			destroyInternal(true);
		}
	}

	HSceneObject SceneObject::create(const String& name)
	{
		HSceneObject newObject = createInternal(name);

		gCoreSceneManager().registerNewSO(newObject);

		return newObject;
	}

	HSceneObject SceneObject::createInternal(const String& name)
	{
		std::shared_ptr<SceneObject> sceneObjectPtr = std::shared_ptr<SceneObject>(new (bs_alloc<SceneObject, PoolAlloc>()) SceneObject(name), 
			&bs_delete<PoolAlloc, SceneObject>, StdAlloc<SceneObject, PoolAlloc>());
		
		HSceneObject sceneObject = GameObjectManager::instance().registerObject(sceneObjectPtr);
		sceneObject->mThisHandle = sceneObject;

		return sceneObject;
	}

	void SceneObject::destroy(bool immediate)
	{
		// Parent is our owner, so when his reference to us is removed, delete might be called.
		// So make sure this is the last thing we do.
		if(mParent != nullptr)
		{
			if(!mParent.isDestroyed())
				mParent->removeChild(mThisHandle);
		}

		destroyInternal(immediate);
	}

	void SceneObject::destroyInternal(bool immediate)
	{
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
			(*iter)->destroyInternal(immediate);

		mChildren.clear();

		for(auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
		{
			gCoreSceneManager().notifyComponentRemoved((*iter));
			(*iter)->onDestroyed();

			if (immediate)
			{
				GameObjectManager::instance().unregisterObject(*iter);
				(*iter).destroy();
			}
			else
				GameObjectManager::instance().queueForDestroy(*iter);
		}

		mComponents.clear();

		if (immediate)
		{
			GameObjectManager::instance().unregisterObject(mThisHandle);
			mThisHandle.destroy();
		}
		else
			GameObjectManager::instance().queueForDestroy(mThisHandle);
	}

	void SceneObject::_setInstanceData(GameObjectInstanceDataPtr& other)
	{
		GameObject::_setInstanceData(other);

		// Instance data changed, so make sure to refresh the handles to reflect that
		mThisHandle._setHandleData(mThisHandle.getInternalPtr());
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

	void SceneObject::setWorldPosition(const Vector3& position)
	{
		if (mParent != nullptr)
		{
			Vector3 invScale = mParent->getWorldScale();
			if (invScale.x != 0) invScale.x = 1.0f / invScale.x;
			if (invScale.y != 0) invScale.y = 1.0f / invScale.y;
			if (invScale.z != 0) invScale.z = 1.0f / invScale.z;

			Quaternion invRotation = mParent->getWorldRotation().inverse();

			mPosition = invRotation.rotate(position - mParent->getWorldPosition()) *  invScale;
		}
		else
			mPosition = position;

		markTfrmDirty();
	}

	void SceneObject::setWorldRotation(const Quaternion& rotation)
	{
		if (mParent != nullptr)
		{
			Quaternion invRotation = mParent->getWorldRotation().inverse();

			mRotation = invRotation * rotation;
		}
		else
			mRotation = rotation;

		markTfrmDirty();
	}

	const Vector3& SceneObject::getWorldPosition() const
	{ 
		if (!isCachedWorldTfrmUpToDate())
			updateWorldTfrm();

		return mWorldPosition; 
	}

	const Quaternion& SceneObject::getWorldRotation() const 
	{ 
		if (!isCachedWorldTfrmUpToDate())
			updateWorldTfrm();

		return mWorldRotation; 
	}

	const Vector3& SceneObject::getWorldScale() const 
	{ 
		if (!isCachedWorldTfrmUpToDate())
			updateWorldTfrm();

		return mWorldScale; 
	}

	void SceneObject::lookAt(const Vector3& location, const Vector3& up)
	{
		Vector3 forward = location - mPosition;
		forward.normalize();

		setForward(forward);

		Quaternion upRot = Quaternion::getRotationFromTo(getUp(), up);
		setRotation(getRotation() * upRot);
	}

	const Matrix4& SceneObject::getWorldTfrm() const
	{
		if (!isCachedWorldTfrmUpToDate())
			updateWorldTfrm();

		return mCachedWorldTfrm;
	}

	const Matrix4& SceneObject::getLocalTfrm() const
	{
		if (!isCachedLocalTfrmUpToDate())
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
		Vector3 trans = mRotation.rotate(vec);

		setPosition(mPosition + trans);
	}

	void SceneObject::rotate(const Vector3& axis, const Radian& angle)
	{
		Quaternion q;
		q.fromAxisAngle(axis, angle);
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
		Vector3 zAxis = mRotation.rotate(Vector3::UNIT_Z);
		rotate(zAxis, angle);
	}

	void SceneObject::yaw(const Radian& angle)
	{
		Vector3 yAxis = mRotation.rotate(Vector3::UNIT_Y);
		rotate(yAxis, angle);
	}

	void SceneObject::pitch(const Radian& angle)
	{
		// Rotate around local X axis
		Vector3 xAxis = mRotation.rotate(Vector3::UNIT_X);
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
			Quaternion rotQuat = Quaternion::getRotationFromTo(currentForwardDir, nrmForwardDir);
			targetRotation = rotQuat * currentRotation;
		}

		setRotation(targetRotation);
	}

	void SceneObject::updateTransformsIfDirty()
	{
		if (!isCachedLocalTfrmUpToDate())
			updateLocalTfrm();

		if (!isCachedWorldTfrmUpToDate())
			updateWorldTfrm();
	}

	void SceneObject::markTfrmDirty() const
	{
		mDirtyFlags |= DirtyFlags::LocalTfrmDirty | DirtyFlags::WorldTfrmDirty;
		mIsCoreDirtyFlags = 0xFFFFFFFF;
		mDirtyHash++;

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
			// Scale own position by parent scale, just combine
			// as equivalent axes, no shearing
			mWorldScale = parentScale * mScale;

			// Change position vector based on parent's orientation & scale
			mWorldPosition = parentOrientation.rotate(parentScale * mPosition);

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

		mDirtyFlags &= ~DirtyFlags::WorldTfrmDirty;
	}

	void SceneObject::updateLocalTfrm() const
	{
		mCachedLocalTfrm.setTRS(mPosition, mRotation, mScale);

		mDirtyFlags &= ~DirtyFlags::LocalTfrmDirty;
	}

	/************************************************************************/
	/* 								Hierarchy	                     		*/
	/************************************************************************/

	void SceneObject::setParent(const HSceneObject& parent)
	{
		if(parent.isDestroyed())
		{
			BS_EXCEPT(InternalErrorException, 
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

	HSceneObject SceneObject::getChild(UINT32 idx) const
	{
		if(idx < 0 || idx >= mChildren.size())
		{
			BS_EXCEPT(InternalErrorException, "Child index out of range.");
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
			BS_EXCEPT(InternalErrorException, 
				"Trying to remove a child but it's not a child of the transform.");
		}
	}

	void SceneObject::setActive(bool active)
	{
		mActiveSelf = active;
		setActiveHierarchy(active);
	}

	void SceneObject::setActiveHierarchy(bool active) 
	{ 
		mActiveHierarchy = active && mActiveSelf; 

		for (auto child : mChildren)
		{
			child->setActiveHierarchy(mActiveHierarchy);
		}

		mIsCoreDirtyFlags = 0xFFFFFFFF;
	}

	bool SceneObject::getActive(bool self)
	{
		if (self)
			return mActiveSelf;
		else
			return mActiveHierarchy;
	}

	HSceneObject SceneObject::clone()
	{
		UINT32 bufferSize = 0;

		MemorySerializer serializer;
		UINT8* buffer = serializer.encode(this, bufferSize, &bs_alloc);

		GameObjectManager::instance().setDeserializationMode(GODM_UseNewIds | GODM_RestoreExternal);
		std::shared_ptr<SceneObject> cloneObj = std::static_pointer_cast<SceneObject>(serializer.decode(buffer, bufferSize));
		bs_free(buffer);

		return cloneObj->mThisHandle;
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

	void SceneObject::destroyComponent(const HComponent& component, bool immediate)
	{
		if(component == nullptr)
		{
			LOGDBG("Trying to remove a null component");
			return;
		}

		auto iter = std::find(mComponents.begin(), mComponents.end(), component);

		if(iter != mComponents.end())
		{
			gCoreSceneManager().notifyComponentRemoved((*iter));
			(*iter)->onDestroyed();
			
			mComponents.erase(iter);

			if (immediate)
			{
				GameObjectManager::instance().unregisterObject(component);
				(*iter).destroy();
			}
			else
			{
				GameObjectManager::instance().queueForDestroy(component);
			}
		}
		else
			LOGDBG("Trying to remove a component that doesn't exist on this SceneObject.");
	}

	void SceneObject::destroyComponent(Component* component, bool immediate)
	{
		auto iterFind = std::find_if(mComponents.begin(), mComponents.end(), 
			[component](const HComponent& x) 
		{ 
			if(x.isDestroyed())
				return false;

			return x._getHandleData()->mPtr->object.get() == component; }
		);

		if(iterFind != mComponents.end())
		{
			destroyComponent(*iterFind, immediate);
		}
	}

	void SceneObject::addComponentInternal(const std::shared_ptr<Component> component)
	{
		GameObjectHandle<Component> newComponent = GameObjectHandle<Component>(component);
		newComponent->mParent = mThisHandle;
		mComponents.push_back(newComponent);

		gCoreSceneManager().notifyComponentAdded(newComponent);
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