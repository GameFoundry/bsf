#include "CmOverlay.h"
#include "CmOverlayManager.h"

namespace CamelotEngine
{
	Overlay::Overlay(const HGameObject& parent)
		:Component(parent)
	{

	}

	Overlay::~Overlay()
	{
		OverlayManager::instance().detachOverlayFromAll(this);
	}
}