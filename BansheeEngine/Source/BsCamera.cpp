#include "BsCamera.h"
#include "BsCameraRTTI.h"
#include "BsMath.h"
#include "BsMatrix3.h"
#include "BsVector2.h"
#include "BsAABox.h"
#include "BsSphere.h"
#include "BsHardwareBufferManager.h"
#include "BsVertexBuffer.h"
#include "BsIndexBuffer.h"
#include "BsException.h"
#include "BsRenderAPI.h"
#include "BsSceneObject.h"
#include "BsDebug.h"
#include "BsSceneManager.h"

namespace BansheeEngine 
{
	Camera::Camera(const HSceneObject& parent, RenderTargetPtr target, float left, float top, float width, float height)
		: Component(parent), mTarget(target), mLeft(left), mTop(top), mWidth(width), mHeight(height)
    {
		setName("Camera");
    }

    Camera::~Camera()
    {
		mInternal->destroy();
    }

	ConvexVolume Camera::getWorldFrustum() const
	{
		const Vector<Plane>& frustumPlanes = getFrustum().getPlanes();
		Matrix4 worldMatrix = SO()->getWorldTfrm();

		Vector<Plane> worldPlanes(frustumPlanes.size());
		UINT32 i = 0;
		for (auto& plane : frustumPlanes)
		{
			worldPlanes[i] = worldMatrix.multiplyAffine(plane);
			i++;
		}

		return ConvexVolume(worldPlanes);
	}

	void Camera::updateView() const
	{
		UINT32 curHash = SO()->getTransformHash();
		if (curHash != mInternal->_getLastModifiedHash())
		{
			mInternal->setPosition(SO()->getWorldPosition());
			mInternal->setRotation(SO()->getWorldRotation());

			mInternal->_setLastModifiedHash(curHash);
		}
	}

	void Camera::update() 
	{

	}

	void Camera::onInitialized()
	{
		// If mInternal already exists this means this object was deserialized,
		// so all we need to do is initialize it.
		if (mInternal != nullptr)
			mInternal->initialize();
		else
		{
			mInternal = CameraHandler::create(mTarget, mLeft, mTop, mWidth, mHeight);
			mTarget = nullptr;
		}

		gSceneManager()._registerCamera(mInternal, SO());
	}

	void Camera::onDestroyed()
	{
		gSceneManager()._unregisterCamera(mInternal);
	}

	RTTITypeBase* Camera::getRTTIStatic()
	{
		return CameraRTTI::instance();
	}

	RTTITypeBase* Camera::getRTTI() const
	{
		return Camera::getRTTIStatic();
	}
} 
