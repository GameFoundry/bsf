//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "BsOverlay.h"

namespace BansheeEngine
{
	bool OverlayManager::OverlayComparer::operator() (const Overlay* const& a, const Overlay* const& b)
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

	void OverlayManager::attachOverlay(const Viewport* target, const Overlay* overlay)
	{
		mOverlaysPerTarget[target].insert(overlay);
	}

	void OverlayManager::detachOverlay(const Viewport* target, const Overlay* overlay)
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