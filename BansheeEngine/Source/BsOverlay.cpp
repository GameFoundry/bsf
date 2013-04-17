#include "BsOverlay.h"
#include "BsOverlayManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
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