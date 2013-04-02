#include "CmOverlayManager.h"
#include "CmCamera.h"
#include "CmOverlay.h"

namespace CamelotEngine
{
	void OverlayManager::render(const CameraPtr& camera, DeferredRenderContextPtr& renderContext) const
	{
		auto overlays = mOverlaysPerCamera.find(camera.get());

		if(overlays == mOverlaysPerCamera.end())
			return;

		for(auto& overlay : overlays->second)
		{
			overlay->render(camera, renderContext);
		}
	}

	void OverlayManager::attachOverlay(const CameraPtr& camera, const Overlay* overlay)
	{
		mOverlaysPerCamera[camera.get()].insert(overlay);
	}

	void OverlayManager::detachOverlay(const CameraPtr& camera, const Overlay* overlay)
	{
		mOverlaysPerCamera[camera.get()].erase(overlay);
	}

	void OverlayManager::detachOverlayFromAll(const Overlay* overlay)
	{
		for(auto& overlays : mOverlaysPerCamera)
		{
			overlays.second.erase(overlay);
		}
	}
}