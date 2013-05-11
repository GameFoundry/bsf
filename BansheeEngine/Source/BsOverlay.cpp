#include "BsOverlay.h"
#include "BsOverlayManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Overlay::Overlay(const HSceneObject& parent)
		:Component(parent), mRenderTarget(nullptr), mDepth(0)
	{

	}

	Overlay::~Overlay()
	{
		OverlayManager::instance().detachOverlayFromAll(this);
	}

	void Overlay::initialize(CM::Viewport* target)
	{
		if(mRenderTarget != nullptr)
			OverlayManager::instance().detachOverlay(mRenderTarget, this);

		mRenderTarget = target;

		OverlayManager::instance().attachOverlay(mRenderTarget, this);	
	}
}