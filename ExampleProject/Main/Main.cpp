#include "stdafx.h"
#include <windows.h>

#include "CmApplication.h"
#include "CmImporter.h"
#include "CmGpuProgramImportOptions.h"
#include "CmMaterial.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmCoreThreadAccessor.h"
#include "BsApplication.h"
#include "BsVirtualInput.h"
#include "BsCamera.h"
#include "BsRenderable.h"
#include "BsGUIWidget.h"
#include "BsGUIArea.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUIListBox.h"
#include "BsBuiltinResources.h"
#include "CmRTTIType.h"
#include "CmHString.h"
#include "CmRenderWindow.h"
#include "CmSceneObject.h"

using namespace BansheeEngine;

Path exampleModelPath = "..\\..\\..\\..\\Data\\Examples\\Pyromancer.fbx";
Path exampleTexturePath = "..\\..\\..\\..\\Data\\Examples\\Pyromancer.psd";
Path exampleFragmentShaderPath = "..\\..\\..\\..\\Data\\Examples\\example_fs.gpuprog";
Path exampleVertexShaderPath = "..\\..\\..\\..\\Data\\Examples\\example_vs.gpuprog";

GUIButton* toggleFullscreenButton = nullptr;
UINT32 resolutionWidth = 1280;
UINT32 resolutionHeight = 720;

HMesh exampleModel;
HTexture exampleTexture;
HGpuProgram exampleFragmentGPUProg;
HGpuProgram exampleVertexGPUProg;

HCamera sceneCamera;

void setUpExample();

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	RENDER_WINDOW_DESC renderWindowDesc;
	renderWindowDesc.width = resolutionWidth;
	renderWindowDesc.height = resolutionHeight;
	renderWindowDesc.title = "Banshee Example App";
	renderWindowDesc.fullscreen = false;

	gBansheeApp().startUp(renderWindowDesc, "CamelotD3D11RenderSystem", "BansheeForwardRenderer"); // TODO - Use enums instead of names. BansheeApp is a high level system that doesn't need to be as customizable.
	setUpExample();
	
	gBansheeApp().runMainLoop();
	gBansheeApp().shutDown();

	return 0;
}

void toggleFullscreen()
{
	RenderWindowPtr window = gApplication().getPrimaryWindow();



	//window->setFullscreen(!window->isFullScreen(), resolutionWidth, resolutionHeight);
}

void setUpExample()
{
	// Import assets
	exampleModel = static_resource_cast<Mesh>(Importer::instance().import(exampleModelPath));
	exampleTexture = static_resource_cast<Texture>(Importer::instance().import(exampleTexturePath));

	ImportOptionsPtr gpuProgImportOptions = Importer::instance().createImportOptions(exampleFragmentShaderPath);
	if(rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
	{
		GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

		importOptions->setEntryPoint("ps_main");
		importOptions->setLanguage("hlsl");
		importOptions->setProfile(GPP_PS_4_0);
		importOptions->setType(GPT_FRAGMENT_PROGRAM);
	}

	exampleFragmentGPUProg = static_resource_cast<GpuProgram>(Importer::instance().import(exampleFragmentShaderPath, gpuProgImportOptions));

	gpuProgImportOptions = Importer::instance().createImportOptions(exampleVertexShaderPath);
	if(rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
	{
		GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

		importOptions->setEntryPoint("vs_main");
		importOptions->setLanguage("hlsl");
		importOptions->setProfile(GPP_VS_4_0);
		importOptions->setType(GPT_VERTEX_PROGRAM);
	}

	exampleVertexGPUProg = static_resource_cast<GpuProgram>(Importer::instance().import(exampleVertexShaderPath, gpuProgImportOptions));

	// TODO - Optionally make the entire import step one-time. After import save resources and the created scene. Then on next load just load them directly from disk.

	// Create material
	ShaderPtr exampleShader = Shader::create("ExampleShader");

	exampleShader->addParameter("matViewProjection", "matViewProjection", GPDT_MATRIX_4X4);
	exampleShader->addParameter("samp", "samp", GPOT_SAMPLER2D);
	exampleShader->addParameter("tex", "tex", GPOT_TEXTURE2D);

	TechniquePtr technique = exampleShader->addTechnique("D3D11RenderSystem", "ForwardRenderer"); // TODO - This render system and forward renderer names should at least match the above names used for initialization
	PassPtr pass = technique->addPass();
	pass->setVertexProgram(exampleVertexGPUProg);
	pass->setFragmentProgram(exampleFragmentGPUProg);

	HMaterial exampleMaterial = Material::create(exampleShader);
	exampleMaterial->setTexture("tex", exampleTexture);

	// Set up the object to render
	HSceneObject pyromancerSO = SceneObject::create("Pyromancer");
	HRenderable renderable = pyromancerSO->addComponent<Renderable>();
	renderable->setMesh(exampleModel);
	renderable->setMaterial(exampleMaterial);

	// Set up scene camera
	HSceneObject sceneCameraGO = SceneObject::create("SceneCamera");

	RenderWindowPtr window = gApplication().getPrimaryWindow(); // TODO - Up until now I'm using gBansheeApp and now I'm using gApplication. It's confusing. BansheeApp should derive from application
	sceneCamera = sceneCameraGO->addComponent<Camera>(window, 0.0f, 0.0f, 1.0f, 1.0f);

	sceneCamera->setPriority(1);
	sceneCameraGO->setPosition(Vector3(0,50,1240));
	sceneCameraGO->lookAt(Vector3(0,50,-300));
	sceneCamera->setNearClipDistance(5);
	sceneCamera->setAspectRatio(resolutionWidth / (float)resolutionHeight); // TODO - This needs to get called whenever resolution changes

	// Register input configuration
	auto inputConfig = VirtualInput::instance().getConfiguration();

	inputConfig->registerButton("Forward", BC_W);
	inputConfig->registerButton("Left", BC_A);
	inputConfig->registerButton("Right", BC_D);
	inputConfig->registerButton("Back", BC_S);

	inputConfig->registerButton("SimThreadProfilerOverlay", BC_F1);
	inputConfig->registerButton("CoreThreadProfilerOverlay", BC_F2);
	inputConfig->registerButton("GPUProfilerOverlay", BC_F3);

	// TODO - Add vertical/horizontal axes here

	HSceneObject exampleSO = SceneObject::create("Example");

	HCamera guiCamera = exampleSO->addComponent<Camera>(window);
	guiCamera->setNearClipDistance(5);
	guiCamera->setAspectRatio(1.0f);
	guiCamera->setIgnoreSceneRenderables(true);

	HGUIWidget gui = exampleSO->addComponent<GUIWidget>();
	gui->setDepth(128);
	gui->setSkin(BuiltinResources::instance().getGUISkin());

	// Profiler overlay GUI
	GUIArea* topArea = GUIArea::createStretchedXY(*gui, 0, 0, 0, 0);
	GUILayout& topLayout = topArea->getLayout().addLayoutY();

	topLayout.addElement(GUILabel::create(HString(L"Press F1 to toggle Sim thread profiler overlay")));
	topLayout.addElement(GUILabel::create(HString(L"Press F2 to toggle Core thread profiler overlay")));
	topLayout.addElement(GUILabel::create(HString(L"Press F3 to toggle GPU profiler overlay")));
	topLayout.addFlexibleSpace();

	// Resolution/Camera options GUI
	GUIArea* rightArea = GUIArea::createStretchedXY(*gui, 0, 0, 0, 0);
	rightArea->getLayout().addFlexibleSpace();
	GUILayout& rightLayout = rightArea->getLayout().addLayoutY();

	rightLayout.addSpace(50);
	toggleFullscreenButton = GUIButton::create(HString(L"Toggle fullscreen"));
	toggleFullscreenButton->onClick.connect(&toggleFullscreen);
	rightLayout.addElement(toggleFullscreenButton);

	// TODO - add ExampleGUI component
}