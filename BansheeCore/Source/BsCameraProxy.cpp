#include "BsCameraProxy.h"

namespace BansheeEngine
{
	void CameraProxy::calcWorldFrustum()
	{
		const Vector<Plane>& frustumPlanes = frustum.getPlanes();
		Vector<Plane> worldPlanes;
		for (auto& plane : frustumPlanes)
		{
			worldPlanes.push_back(worldMatrix.multiply3x4(plane));
		}

		worldFrustum = ConvexVolume(worldPlanes);
	}
}