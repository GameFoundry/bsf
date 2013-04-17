#include "CmOverlay.h"
#include "CmOverlayManager.h"

namespace CamelotFramework
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