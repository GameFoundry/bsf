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
		: Component(parent), mLastUpdateHash(std::numeric_limits<UINT32>::max())
    {
		mInternal = CameraHandler::create(target, left, top, width, height);
		gSceneManager()._registerCamera(mInternal, parent);

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
		if (curHash != mLastUpdateHash)
		{
			mInternal->setPosition(SO()->getWorldPosition());
			mInternal->setRotation(SO()->getWorldRotation());

			mLastUpdateHash = curHash;
		}
	}

	void Camera::update() 
	{
		updateView();
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
