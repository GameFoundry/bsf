//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCSkybox.h"
#include "BsCSkyboxRTTI.h"
#include "BsSceneManager.h"
#include "BsSkybox.h"

namespace bs
{
	CSkybox::CSkybox()
	{
		setFlag(ComponentFlag::AlwaysRun, true);
		setName("Skybox");
	}

	CSkybox::CSkybox(const HSceneObject& parent)
		: Component(parent)
	{
		setFlag(ComponentFlag::AlwaysRun, true);
		setName("Skybox");
	}

	CSkybox::~CSkybox()
	{
		mInternal->destroy();
	}

	void CSkybox::onInitialized()
	{
		// If mInternal already exists this means this object was deserialized,
		// so all we need to do is initialize it.
		if (mInternal != nullptr)
			mInternal->initialize();
		else
			mInternal = Skybox::create();
	}

	RTTITypeBase* CSkybox::getRTTIStatic()
	{
		return CSkyboxRTTI::instance();
	}

	RTTITypeBase* CSkybox::getRTTI() const
	{
		return CSkybox::getRTTIStatic();
	}
}
