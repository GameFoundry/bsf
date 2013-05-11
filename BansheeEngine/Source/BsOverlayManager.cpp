#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "BsOverlay.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	bool OverlayManager::OverlayComparer::operator() (const Overlay* const& a, const Overlay* const& b)
	{
		return a->getDepth() > b->getDepth();
	}

	void OverlayManager::render(CM::ViewportPtr& target, RenderContext& renderContext) const
	{
		auto overlays = mOverlaysPerTarget.find(target.get());

		if(overlays == mOverlaysPerTarget.end())
			return;

		renderContext.setViewport(target);
		
		// Render all overlays. They should already be sorted by depth, front most rendering last
		for(auto& overlay : overlays->second)
		{
			overlay->render(renderContext);
		}
	}

	void OverlayManager::attachOverlay(const CM::Viewport* target, const Overlay* overlay)
	{
		mOverlaysPerTarget[target].insert(overlay);
	}

	void OverlayManager::detachOverlay(const CM::Viewport* target, const Overlay* overlay)
	{
		mOverlaysPerTarget[target].erase(overlay);
	}

	void OverlayManager::detachOverlayFromAll(const Overlay* overlay)
	{
		for(auto& overlays : mOverlaysPerTarget)
		{
			overlays.second.erase(overlay);
		}
	}
}