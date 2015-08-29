#include "BsOverlayManager.h"
#include "BsCCamera.h"
#include "BsCOverlay.h"

namespace BansheeEngine
{
	bool OverlayManager::OverlayComparer::operator() (const COverlay* const& a, const COverlay* const& b)
	{
		return a->getDepth() > b->getDepth();
	}

	void OverlayManager::render(ViewportPtr& target, DrawList& drawList) const
	{
		auto overlays = mOverlaysPerTarget.find(target.get());

		if(overlays == mOverlaysPerTarget.end())
			return;

		// Render all overlays. They should already be sorted by depth, front most rendering last
		for(auto& overlay : overlays->second)
		{
			overlay->render(drawList);
		}
	}

	void OverlayManager::attachOverlay(const Viewport* target, const COverlay* overlay)
	{
		mOverlaysPerTarget[target].insert(overlay);
	}

	void OverlayManager::detachOverlay(const Viewport* target, const COverlay* overlay)
	{
		mOverlaysPerTarget[target].erase(overlay);
	}

	void OverlayManager::detachOverlayFromAll(const COverlay* overlay)
	{
		for(auto& overlays : mOverlaysPerTarget)
		{
			overlays.second.erase(overlay);
		}
	}
}