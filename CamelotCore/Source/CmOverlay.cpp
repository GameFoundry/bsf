#include "CmOverlay.h"
#include "CmOverlayManager.h"

namespace CamelotEngine
{
	Overlay::Overlay(GameObjectPtr parent)
		:Component(parent)
	{

	}

	Overlay::~Overlay()
	{
		OverlayManager::instance().detachOverlayFromAll(this);
	}
}