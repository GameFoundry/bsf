//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Testing/BsTestSuite.h"
// #include "Platform/BsPlatform.h"
// #include "Utility/BsTime.h"
#include "BsApplication.h"
#include "BsEngineConfig.h"
#include "./BsTestCameraFlyer.h"
#include "Components/BsCCamera.h"
#include "Scene/BsSceneObject.h"

namespace bs
{

void setupInputConfig();

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

	setupInputConfig();
}

void AppEnvironment::TearDown() {
	bs::Application::shutDown();
}

HSceneObject Test::addFlyableCamera() {

	HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
	HCamera sceneCamera = sceneCameraSO->addComponent<CCamera>();
	sceneCameraSO->addComponent<CameraFlyer>();
	sceneCamera->setMain(true);
	sceneCameraSO->setPosition(Vector3(10.0f, 10.0f, 10.0f));
	sceneCameraSO->lookAt(Vector3(0, 0, 0));

	return sceneCameraSO;
}

void setupInputConfig()
{
	// Register input configuration
	// bsf allows you to use VirtualInput system which will map input device buttons and axes to arbitrary names,
	// which allows you to change input buttons without affecting the code that uses it, since the code is only
	// aware of the virtual names.  If you want more direct input, see Input class.
	auto inputConfig = gVirtualInput().getConfiguration();

	// Camera controls for buttons (digital 0-1 input, e.g. keyboard or gamepad button)
	inputConfig->registerButton("Forward", BC_W);
	inputConfig->registerButton("Back", BC_S);
	inputConfig->registerButton("Left", BC_A);
	inputConfig->registerButton("Right", BC_D);
	inputConfig->registerButton("Forward", BC_UP);
	inputConfig->registerButton("Back", BC_DOWN);
	inputConfig->registerButton("Left", BC_LEFT);
	inputConfig->registerButton("Right", BC_RIGHT);
	inputConfig->registerButton("FastMove", BC_LSHIFT);
	inputConfig->registerButton("RotateObj", BC_MOUSE_LEFT);
	inputConfig->registerButton("RotateCam", BC_MOUSE_RIGHT);

	// Camera controls for axes (analog input, e.g. mouse or gamepad thumbstick)
	// These return values in [-1.0, 1.0] range.
	inputConfig->registerAxis("Horizontal", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
	inputConfig->registerAxis("Vertical", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
}

} // namespace bs
