//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsOverlay.h"
#include "BsOverlayManager.h"

namespace BansheeEngine
{
	Overlay::Overlay(const HSceneObject& parent, Viewport* target)
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