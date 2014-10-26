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
#include "BsRenderSystem.h"
#include "BsSceneObject.h"
#include "BsDebug.h"

namespace BansheeEngine 
{
	Camera::Camera(const HSceneObject& parent, RenderTargetPtr target, float left, float top, float width, float height)
		: Component(parent), mLastUpdateHash(std::numeric_limits<UINT32>::max())
    {
		mInternal = bs_shared_ptr<CameraHandler>(target, left, top, width, height);

		setName("Camera");
    }

    Camera::~Camera()
    {
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

	RTTITypeBase* Camera::getRTTIStatic()
	{
		return CameraRTTI::instance();
	}

	RTTITypeBase* Camera::getRTTI() const
	{
		return Camera::getRTTIStatic();
	}
} 
