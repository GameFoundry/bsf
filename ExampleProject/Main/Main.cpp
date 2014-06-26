#include <windows.h>

#include "BsApplication.h"
#include "BsImporter.h"
#include "BsGpuProgramImportOptions.h"
#include "BsMaterial.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsCoreThreadAccessor.h"
#include "BsApplication.h"
#include "BsVirtualInput.h"
#include "BsCamera.h"
#include "BsRenderable.h"
#include "BsGUIWidget.h"
#include "BsGUIArea.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUISpace.h"
#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUIListBox.h"
#include "BsBuiltinResources.h"
#include "BsRTTIType.h"
#include "BsHString.h"
#include "BsRenderWindow.h"
#include "BsSceneObject.h"
#include "BsCoreThread.h"
#include "BsProfilerOverlay.h"
#include "BsRenderer.h"

#include "CameraFlyer.h"

namespace BansheeEngine
{
	Path exampleModelPath = "..\\..\\..\\..\\Data\\Examples\\Pyromancer.fbx";
	Path exampleTexturePath = "..\\..\\..\\..\\Data\\Examples\\Pyromancer.psd";
	Path exampleFragmentShaderPath = "..\\..\\..\\..\\Data\\Examples\\example_fs.gpuprog";
	Path exampleVertexShaderPath = "..\\..\\..\\..\\Data\\Examples\\example_vs.gpuprog";

	GUIButton* toggleFullscreenButton = nullptr;
	UINT32 resolutionWidth = 1280;
	UINT32 resolutionHeight = 720;
	bool fullscreen = false;
	const VideoMode* videoMode = nullptr;

	HMesh exampleModel;
	HTexture exampleTexture;
	HGpuProgram exampleFragmentGPUProg;
	HGpuProgram exampleVertexGPUProg;

	HCamera sceneCamera;
	HProfilerOverlay profilerOverlay;

	VirtualButton toggleCPUProfilerBtn;
	VirtualButton toggleGPUProfilerBtn;

	bool cpuProfilerActive = false;
	bool gpuProfilerActive = false;

	void toggleFullscreen()
	{
		RenderWindowPtr window = gApplication().getPrimaryWindow();

		if (fullscreen)
		{
			gCoreAccessor().setWindowed(window, resolutionWidth, resolutionHeight);
		}
		else
		{
			//gCoreAccessor().setFullscreen(window, *videoMode);
			gCoreAccessor().setFullscreen(window, 1920, 1200);
		}

		fullscreen = !fullscreen;
	}

	void buttonUp(const VirtualButton& button, UINT32 deviceIdx)
	{
		if (button == toggleCPUProfilerBtn)
		{
			if (cpuProfilerActive)
			{
				profilerOverlay->hide();
				cpuProfilerActive = false;
			}
			else
			{
				profilerOverlay->show(ProfilerOverlayType::CPUSamples);
				cpuProfilerActive = true;
				gpuProfilerActive = false;
			}
		}
		else if (button == toggleGPUProfilerBtn)
		{
			if (gpuProfilerActive)
			{
				profilerOverlay->hide();
				gpuProfilerActive = false;
			}
			else
			{
				profilerOverlay->show(ProfilerOverlayType::GPUSamples);
				gpuProfilerActive = true;
				cpuProfilerActive = false;
			}
		}
	}

	void setUpExample()
	{
		// Import assets
		exampleModel = static_resource_cast<Mesh>(Importer::instance().import(exampleModelPath));
		exampleTexture = static_resource_cast<Texture>(Importer::instance().import(exampleTexturePath));

		ImportOptionsPtr gpuProgImportOptions = Importer::instance().createImportOptions(exampleFragmentShaderPath);
		if (rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
		{
			GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

			importOptions->setEntryPoint("ps_main");
			importOptions->setLanguage("hlsl");
			importOptions->setProfile(GPP_PS_4_0);
			importOptions->setType(GPT_FRAGMENT_PROGRAM);
		}

		exampleFragmentGPUProg = static_resource_cast<GpuProgram>(Importer::instance().import(exampleFragmentShaderPath, gpuProgImportOptions));

		gpuProgImportOptions = Importer::instance().createImportOptions(exampleVertexShaderPath);
		if (rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
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

		// Set up shader parameters and renderer semantics
		exampleShader->setParamBlockAttribs("PerObject", true, GPBU_DYNAMIC, RBS_PerObject);
		exampleShader->addParameter("matWorldViewProj", "matWorldViewProj", GPDT_MATRIX_4X4, RPS_WorldViewProjTfrm);
		exampleShader->addParameter("samp", "samp", GPOT_SAMPLER2D);
		exampleShader->addParameter("tex", "tex", GPOT_TEXTURE2D);

		TechniquePtr technique = exampleShader->addTechnique(RenderSystemDX11, RendererDefault); // TODO - This render system and forward renderer names should at least match the above names used for initialization
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
		HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");

		RenderWindowPtr window = gApplication().getPrimaryWindow();
		sceneCamera = sceneCameraSO->addComponent<Camera>(window);

		sceneCamera->setPriority(1);
		sceneCameraSO->setPosition(Vector3(40.0f, 30.0f, 230.0f));
		sceneCameraSO->lookAt(Vector3(0, 0, 0));
		sceneCamera->setNearClipDistance(5);
		sceneCamera->setAspectRatio(resolutionWidth / (float)resolutionHeight); // TODO - This needs to get called whenever resolution changes

		sceneCameraSO->addComponent<CameraFlyer>();

		// Register input configuration
		auto inputConfig = VirtualInput::instance().getConfiguration();

		inputConfig->registerButton("Forward", BC_W);
		inputConfig->registerButton("Back", BC_S);
		inputConfig->registerButton("Left", BC_A);
		inputConfig->registerButton("Right", BC_D);
		inputConfig->registerButton("Forward", BC_UP);
		inputConfig->registerButton("Back", BC_BACK);
		inputConfig->registerButton("Left", BC_LEFT);
		inputConfig->registerButton("Right", BC_RIGHT);
		inputConfig->registerButton("FastMove", BC_LSHIFT);
		inputConfig->registerButton("RotateCam", BC_MOUSE_RIGHT);
		inputConfig->registerAxis("Horizontal", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
		inputConfig->registerAxis("Vertical", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));

		inputConfig->registerButton("CPUProfilerOverlay", BC_F1);
		inputConfig->registerButton("GPUProfilerOverlay", BC_F2);

		toggleCPUProfilerBtn = VirtualButton("CPUProfilerOverlay");
		toggleGPUProfilerBtn = VirtualButton("GPUProfilerOverlay");

		VirtualInput::instance().onButtonUp.connect(&buttonUp);

		HSceneObject exampleSO = SceneObject::create("Example");

		HCamera guiCamera = exampleSO->addComponent<Camera>(window);
		guiCamera->setNearClipDistance(5);
		guiCamera->setAspectRatio(1.0f);
		guiCamera->setIgnoreSceneRenderables(true);
		guiCamera->getViewport()->setRequiresClear(false, false, false);

		HGUIWidget gui = exampleSO->addComponent<GUIWidget>(guiCamera->getViewport().get());
		gui->setDepth(128);
		gui->setSkin(BuiltinResources::instance().getGUISkin());

		// Profiler overlay GUI
		GUIArea* topArea = GUIArea::createStretchedXY(*gui, 0, 0, 0, 0);
		GUILayout& topLayout = topArea->getLayout().addLayoutY();

		topLayout.addElement(GUILabel::create(HString(L"Press F1 to toggle CPU profiler overlay")));
		topLayout.addElement(GUILabel::create(HString(L"Press F2 to toggle GPU profiler overlay")));
		topLayout.addFlexibleSpace();

		// Resolution/Camera options GUI
		GUIArea* rightArea = GUIArea::createStretchedXY(*gui, 0, 0, 0, 0);
		rightArea->getLayout().addFlexibleSpace();
		GUILayout& rightLayout = rightArea->getLayout().addLayoutY();

		rightLayout.addSpace(50);
		toggleFullscreenButton = GUIButton::create(HString(L"Toggle fullscreen"));
		toggleFullscreenButton->onClick.connect(&toggleFullscreen);
		rightLayout.addElement(toggleFullscreenButton);

		// Initialize profiler overlay
		profilerOverlay = exampleSO->addComponent<ProfilerOverlay>(guiCamera->getViewport());

		// TODO - add ExampleGUI component
	}
}

using namespace BansheeEngine;

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	RENDER_WINDOW_DESC renderWindowDesc;
	renderWindowDesc.videoMode = VideoMode(resolutionWidth, resolutionHeight);
	renderWindowDesc.title = "Banshee Example App";
	renderWindowDesc.fullscreen = false;

	Application::startUp(renderWindowDesc, RenderSystemPlugin::DX11, RendererPlugin::Default);
	setUpExample();
	
	Application::instance().runMainLoop();
	Application::shutDown();

	return 0;
}

