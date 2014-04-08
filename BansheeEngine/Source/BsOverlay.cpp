#include "BsOverlay.h"
#include "BsOverlayManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Overlay::Overlay(const HSceneObject& parent, CM::Viewport* target)
		:Component(parent), mRenderTarget(nullptr), mDepth(0)
	{
		setName("Overlay");

		if(mRenderTarget != nullptr)
			OverlayManager::instance().detachOverlay(mRenderTarget, this);

		mRenderTarget = target;

		OverlayManager::instance().attachOverlay(mRenderTarget, this);	
	}

	Overlay::~Overlay()
	{
		OverlayManager::instance().detachOverlayFromAll(this);
	}
}