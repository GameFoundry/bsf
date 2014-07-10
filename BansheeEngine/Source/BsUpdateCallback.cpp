//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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