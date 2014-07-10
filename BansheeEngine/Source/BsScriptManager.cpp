//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsScriptManager.h"

namespace BansheeEngine
{
	void ScriptManager::initialize(const std::shared_ptr<ScriptSystem>& scriptSystem)
	{
		mScriptSystem = scriptSystem;
		mScriptSystem->initialize();
	}

	void ScriptManager::destroy()
	{
		if (mScriptSystem != nullptr)
			mScriptSystem->destroy();
	}

	void ScriptManager::onShutDown()
	{
		mScriptSystem = nullptr;
	}
}