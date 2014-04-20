#include "BsUpdateCallback.h"

namespace BansheeEngine
{
	UpdateCallback::UpdateCallback(const HSceneObject& parent)
		:Component(parent)
	{
		setName("UpdateCallback");
	}

	UpdateCallback::~UpdateCallback()
	{

	}

	void UpdateCallback::update()
	{
		if(!onUpdate.empty())
		{
			onUpdate();
		}
	}
}