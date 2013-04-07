#include "CmOverlayManager.h"
#include "CmCamera.h"
#include "CmOverlay.h"

namespace CamelotEngine
{
	void OverlayManager::render(const Camera* camera, DeferredRenderContextPtr& renderContext) const
	{
		auto overlays = mOverlaysPerCamera.find(camera);

		if(overlays == mOverlaysPerCamera.end())
			return;

		for(auto& overlay : overlays->second)
		{
			overlay->render(camera, renderContext);
		}
	}

	void OverlayManager::attachOverlay(const Camera* camera, const Overlay* overlay)
	{
		mOverlaysPerCamera[camera].insert(overlay);
	}

	void OverlayManager::detachOverlay(const Camera* camera, const Overlay* overlay)
	{
		mOverlaysPerCamera[camera].erase(overlay);
	}

	void OverlayManager::detachOverlayFromAll(const Overlay* overlay)
	{
		for(auto& overlays : mOverlaysPerCamera)
		{
			overlays.second.erase(overlay);
		}
	}
}