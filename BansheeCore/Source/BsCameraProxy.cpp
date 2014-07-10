//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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