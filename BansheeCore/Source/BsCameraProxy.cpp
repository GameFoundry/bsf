#include "BsCameraProxy.h"

namespace BansheeEngine
{
	void CameraProxy::calcWorldFrustum()
	{
		const Vector<Plane>& frustumPlanes = frustum.getPlanes();
		Vector<Plane> worldPlanes;
		for (auto& plane : frustumPlanes)
		{
			worldPlanes.push_back(worldMatrix.multiplyAffine(plane));
		}

		worldFrustum = ConvexVolume(worldPlanes);
	}
}