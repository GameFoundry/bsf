#include "BsCOverlay.h"
#include "BsOverlayManager.h"

namespace BansheeEngine
{
	COverlay::COverlay(const HSceneObject& parent, Viewport* target)
		:Component(parent), mRenderTarget(nullptr), mDepth(0)
	{
		setName("Overlay");

		if(mRenderTarget != nullptr)
			OverlayManager::instance().detachOverlay(mRenderTarget, this);

		mRenderTarget = target;

		OverlayManager::instance().attachOverlay(mRenderTarget, this);	
	}

	COverlay::~COverlay()
	{
		OverlayManager::instance().detachOverlayFromAll(this);
	}
}