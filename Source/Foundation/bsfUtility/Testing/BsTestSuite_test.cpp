//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Testing/BsTestSuite.h"
// #include "Platform/BsPlatform.h"
// #include "Utility/BsTime.h"
#include "BsApplication.h"
#include "BsEngineConfig.h"

namespace bs
{
	START_UP_DESC testStartupDesc()
	{
		START_UP_DESC desc;

		// Set up default plugins
		desc.renderAPI = BS_RENDER_API_MODULE;
		// desc.renderAPI = "NullRenderAPI"
		desc.renderer = BS_RENDERER_MODULE;
		// desc.renderer = "NullRenderer";
		desc.audio = BS_AUDIO_MODULE;
		desc.physics = BS_PHYSICS_MODULE;

		desc.importers.push_back("bsfFreeImgImporter");
		desc.importers.push_back("bsfFBXImporter");
		desc.importers.push_back("bsfFontImporter");
		desc.importers.push_back("bsfSL");

		VideoMode mode;
		desc.primaryWindowDesc.videoMode = mode;
		desc.primaryWindowDesc.fullscreen = false;
		desc.primaryWindowDesc.title = "test";
		return desc;
	}


	void AppEnvironment::SetUp() {

		auto desc = testStartupDesc();
		bs::Application::startUp(desc);
	}

	void AppEnvironment::TearDown() {
		bs::Application::shutDown();
	}

} // namespace bs
