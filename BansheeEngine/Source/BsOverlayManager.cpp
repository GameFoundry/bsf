#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "BsOverlay.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void OverlayManager::render(const Camera* camera, RenderContext& renderContext) const
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