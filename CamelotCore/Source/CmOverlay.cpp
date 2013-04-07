#include "CmOverlay.h"
#include "CmOverlayManager.h"

namespace CamelotEngine
{
	Overlay::Overlay(const HSceneObject& parent)
		:Component(parent)
	{

	}

	Overlay::~Overlay()
	{
		OverlayManager::instance().detachOverlayFromAll(this);
	}
}