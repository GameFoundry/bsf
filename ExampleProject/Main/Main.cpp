#include <windows.h>

#include "BsApplication.h"
#include "BsImporter.h"
#include "BsTextureImportOptions.h"
#include "BsMaterial.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsCoreThreadAccessor.h"
#include "BsApplication.h"
#include "BsVirtualInput.h"
#include "BsCCamera.h"
#include "BsCRenderable.h"
#include "BsCGUIWidget.h"
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
#include "BsCoreRenderer.h"
#include "BsResources.h"
#include "BsGUIPanel.h"

#include "CameraFlyer.h"

namespace BansheeEngine
{
	UINT32 windowResWidth = 1280;
	UINT32 windowResHeight = 720;

	/**
	 * Imports all of our assets and prepares GameObject that handle the example logic.
	 */
	void setUpExample();

	/**
	 * Import mesh/texture/GPU programs used by the example.
	 */
	void importAssets(HMesh& model, HTexture& texture, HShader& shader);

	/**
	 * Create a material used by our example model.
	 */
	HMaterial createMaterial(const HTexture& texture, const HShader& shader);

	/**
	 * Set up example scene objects.
	 */
	void setUp3DScene(const HMesh& mesh, const HMaterial& material);

	/**
	 * Set up example GUI.
	 */
	void setUpGUI();

	/**
	 * Set up input configuration and callbacks.
	 */
	void setUpInput();

	/**
	 * Toggles the primary window between full-screen and windowed mode.
	 */
	void toggleFullscreen();

	/**
	 * Called whenever the main render window is resized.
	 */
	void renderWindowResized();

	/**
	 * Called when the selected video mode changes in the video mode list box.
	 */
	void videoModeChanged(UINT32 idx);

	/**
	 * Triggered whenever a virtual button is released.
	 */
	void buttonUp(const VirtualButton& button, UINT32 deviceIdx);
}

using namespace BansheeEngine;

/**
 * Main entry point into the application.
 */
int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	// Descriptor used for initializing the primary application window.
	RENDER_WINDOW_DESC renderWindowDesc;
	renderWindowDesc.videoMode = VideoMode(windowResWidth, windowResHeight);
	renderWindowDesc.title = "Banshee Example App";
	renderWindowDesc.fullscreen = false;

	// Initializes the application with primary window defined as above and DirectX 11 render system.
	// You may use other render systems than DirectX 11, however this example for simplicity only uses DirectX 11.
	// If you wanted other render systems you would need to create separate shaders for them and import them
	// along with (or replace) the DX11 ones.
	Application::startUp(renderWindowDesc, RenderAPIPlugin::DX11);

	// Imports all of ours assets and prepares GameObject that handle the example logic.
	setUpExample();
	
	// Runs the main loop that does most of the work. This method will exit when user closes the main
	// window or exits in some other way.
	Application::instance().runMainLoop();

	Application::shutDown();

	return 0;
}

namespace BansheeEngine
{
	Path exampleModelPath = RUNTIME_DATA_PATH + "Examples\\Dragon.fbx";
	Path exampleTexturePath = RUNTIME_DATA_PATH + "Examples\\Dragon.tga";
	Path exampleShaderPath = RUNTIME_DATA_PATH + "Examples\\Example.bsl";

	GUIButton* toggleFullscreenButton = nullptr;
	bool fullscreen = false;

	const VideoMode* selectedVideoMode = nullptr;
	Vector<const VideoMode*> videoModes;

	HCamera sceneCamera;
	HProfilerOverlay profilerOverlay;

	VirtualButton toggleCPUProfilerBtn;
	VirtualButton toggleGPUProfilerBtn;

	bool cpuProfilerActive = false;
	bool gpuProfilerActive = false;

	void setUpExample()
	{
		HMesh exampleModel;
		HTexture exampleTexture;
		HShader exampleShader;

		importAssets(exampleModel, exampleTexture, exampleShader);
		HMaterial exampleMaterial = createMaterial(exampleTexture, exampleShader);

		setUp3DScene(exampleModel, exampleMaterial);
		setUpGUI();
		setUpInput();
	}

	void importAssets(HMesh& model, HTexture& texture, HShader& shader)
	{
		// Import mesh, texture and shader from the disk. In a normal application you would want to save the imported assets
		// so next time the application is ran you can just load them directly. This can be done with Resources::save/load.

		// Import an FBX mesh.
		model = Importer::instance().import<Mesh>(exampleModelPath);

		// When importing you may specify optional import options that control how is the asset imported.
		ImportOptionsPtr textureImportOptions = Importer::instance().createImportOptions(exampleTexturePath);

		// rtti_is_of_type checks if the import options are of valid type, in case the provided path is pointing to a non-texture resource.
		// This is similar to dynamic_cast but uses Banshee internal RTTI system for type checking.
		if (rtti_is_of_type<TextureImportOptions>(textureImportOptions))
		{
			TextureImportOptions* importOptions = static_cast<TextureImportOptions*>(textureImportOptions.get());

			// We want maximum number of mipmaps to be generated
			importOptions->setGenerateMipmaps(true);
		}

		// Import texture with specified import options
		texture = Importer::instance().import<Texture>(exampleTexturePath, textureImportOptions);

		// Import shader
		shader = Importer::instance().import<Shader>(exampleShaderPath);
	}

	HMaterial createMaterial(const HTexture& texture, const HShader& shader)
	{
		// And finally create a material with the newly created shader
		HMaterial exampleMaterial = Material::create(shader);

		// And set the texture to be used by the "tex" shader parameter. We leave the "samp"
		// parameter at its defaults.
		exampleMaterial->setTexture("tex", texture);

		return exampleMaterial;
	}

	void setUp3DScene(const HMesh& mesh, const HMaterial& material)
	{
		/************************************************************************/
		/* 								SCENE OBJECT                      		*/
		/************************************************************************/

		// Now we create a scene object that has a position, orientation, scale and optionally
		// components to govern its logic. In this particular case we are creating a SceneObject
		// with a Renderable component which will render a mesh at the position of the scene object
		// with the provided material.

		// Create new scene object at (0, 0, 0)
		HSceneObject dragonSO = SceneObject::create("Dragon");

		// Attach the Renderable component and hook up the mesh we imported earlier,
		// and the material we created in the previous section.
		HRenderable renderable = dragonSO->addComponent<CRenderable>();
		renderable->setMesh(mesh);
		renderable->setMaterial(material);

		/************************************************************************/
		/* 									CAMERA	                     		*/
		/************************************************************************/

		// In order something to render on screen we need at least one camera.

		// Like before, we create a new scene object at (0, 0, 0).
		HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");

		// Get the primary render window we need for creating the camera. Additionally
		// hook up a callback so we are notified when user resizes the window.
		RenderWindowPtr window = gApplication().getPrimaryWindow();
		window->onResized.connect(&renderWindowResized);

		// Add a Camera component that will output whatever it sees into that window 
		// (You could also use a render texture or another window you created).
		sceneCamera = sceneCameraSO->addComponent<CCamera>(window);

		// Set up camera component properties

		// Priority determines in what order are cameras rendered in case multiple cameras render to the same render target.
		// We raise the priority slightly because later in code we have defined a GUI camera that we want to render second.
		sceneCamera->setPriority(1);

		// Set closest distance that is visible. Anything below that is clipped.
		sceneCamera->setNearClipDistance(5);

		// Set aspect ratio depending on the current resolution
		sceneCamera->setAspectRatio(windowResWidth / (float)windowResHeight);

		// Add a CameraFlyer component that allows us to move the camera. See CameraFlyer for more information.
		sceneCameraSO->addComponent<CameraFlyer>();

		// Position and orient the camera scene object
		sceneCameraSO->setPosition(Vector3(-130.0f, 140.0f, 650.0f));
		sceneCameraSO->lookAt(Vector3(0, 0, 0));
	}

	void setUpInput()
	{
		// Register input configuration
		// Banshee allows you to use VirtualInput system which will map input device buttons
		// and axes to arbitrary names, which allows you to change input buttons without affecting
		// the code that uses it, since the code is only aware of the virtual names. 
		// If you want more direct input, see Input class.
		auto inputConfig = VirtualInput::instance().getConfiguration();

		// Camera controls for buttons (digital 0-1 input, e.g. keyboard or gamepad button)
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

		// Camera controls for axes (analog input, e.g. mouse or gamepad thumbstick)
		// These return values in [-1.0, 1.0] range.
		inputConfig->registerAxis("Horizontal", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
		inputConfig->registerAxis("Vertical", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));

		// Controls that toggle the profiler overlays
		inputConfig->registerButton("CPUProfilerOverlay", BC_F1);
		inputConfig->registerButton("GPUProfilerOverlay", BC_F2);

		// Cache the profiler overlay buttons so when a button is pressed we can quickly
		// use these to determine its the one we want
		toggleCPUProfilerBtn = VirtualButton("CPUProfilerOverlay");
		toggleGPUProfilerBtn = VirtualButton("GPUProfilerOverlay");

		// Hook up a callback that gets triggered whenever a virtual button is released
		VirtualInput::instance().onButtonUp.connect(&buttonUp);
	}

	void setUpGUI()
	{
		// Create a scene object that will contain GUI components
		HSceneObject guiSO = SceneObject::create("Example");

		// Get the primary render window we need for creating the camera. 
		RenderWindowPtr window = gApplication().getPrimaryWindow();

		// First we want another camera that is responsible for rendering GUI
		HCamera guiCamera = guiSO->addComponent<CCamera>(window);

		// Set up GUI camera properties. 
		// We don't care about aspect ratio for GUI camera.
		guiCamera->setAspectRatio(1.0f);

		// This camera should ignore any Renderable objects in the scene
		guiCamera->setLayers(0);

		// Don't clear this camera as that would clear anything the main camera has rendered.
		guiCamera->getViewport()->setRequiresClear(false, false, false);

		// Add a GUIWidget, the top-level GUI component, parent to all GUI elements. GUI widgets
		// require you to specify a viewport that they will output rendered GUI elements to.
		HGUIWidget gui = guiSO->addComponent<CGUIWidget>(guiCamera);

		// Depth allows you to control how is a GUI widget rendered in relation to other widgets
		// Lower depth means the widget will be rendered in front of those with higher. In this case we just
		// make the depth mid-range as there are no other widgets.
		gui->setDepth(128);

		// GUI skin defines how are all child elements of the GUI widget renderered. It contains all their styles
		// and default layout properties. We use the default skin that comes built into Banshee.
		gui->setSkin(BuiltinResources::instance().getGUISkin());

		// Get the primary GUI panel that stretches over the entire window and add to it a vertical layout
		// that will be using for vertically positioning messages about toggling profiler overlay.
		GUILayout* bottomLayout = gui->getPanel()->addNewElement<GUILayoutY>();

		// Add a flexible space that fills up any remaining area in the layout, making the two labels above be aligned
		// to the bottom of the GUI widget (and the screen).
		bottomLayout->addNewElement<GUIFlexibleSpace>();

		// Add a couple of labels to the layout with the needed messages. Labels expect a HString object that
		// maps into a string table and allows for easily localization. 
		bottomLayout->addElement(GUILabel::create(HString(L"Press F1 to toggle CPU profiler overlay")));
		bottomLayout->addElement(GUILabel::create(HString(L"Press F2 to toggle GPU profiler overlay")));

		// Create a GUI panel that is used for displaying resolution and fullscreen options.
		GUILayout* rightLayout = gui->getPanel()->addNewElement<GUILayoutX>();
		rightLayout->setPosition(30, 30);

		// We want all the GUI elements be right aligned, so we add a flexible space first.
		rightLayout->addNewElement<GUIFlexibleSpace>();

		// And we want the elements to be vertically placed, top to bottom
		GUILayout* elemLayout = rightLayout->addNewElement<GUILayoutY>();

		// Add a button that will trigger a callback when clicked
		toggleFullscreenButton = GUIButton::create(HString(L"Toggle fullscreen"));
		toggleFullscreenButton->onClick.connect(&toggleFullscreen);
		elemLayout->addElement(toggleFullscreenButton);

		// Leave 30 pixels to the right free
		rightLayout->addNewElement<GUIFixedSpace>(30);

		// Add a profiler overlay object that is responsible for displaying CPU and GPU profiling GUI
		profilerOverlay = guiSO->addComponent<ProfilerOverlay>(guiCamera);

		// Set up video mode list box
		// First get a list of output devices
		const VideoModeInfo& videoModeInfo = RenderAPI::getVideoModeInfo();

		// Get video mode info for the primary monitor
		const VideoOutputInfo& primaryMonitorInfo = videoModeInfo.getOutputInfo(0);

		// Make the current desktop mode the default video mode
		selectedVideoMode = &primaryMonitorInfo.getDesktopVideoMode();

		// Create list box elements for each available video mode
		UINT32 numVideoModes = primaryMonitorInfo.getNumVideoModes();
		Vector<HString> videoModeLabels(numVideoModes);
		UINT32 selectedVideoModeIdx = 0;
		for (UINT32 i = 0; i < numVideoModes; i++)
		{
			const VideoMode& curVideoMode = primaryMonitorInfo.getVideoMode(i);

			HString videoModeLabel(L"{0} x {1} at {2}Hz");
			videoModeLabel.setParameter(0, toWString(curVideoMode.getWidth()));
			videoModeLabel.setParameter(1, toWString(curVideoMode.getHeight()));
			videoModeLabel.setParameter(2, toWString(Math::roundToInt(curVideoMode.getRefreshRate())));

			videoModeLabels[i] = videoModeLabel;
			videoModes.push_back(&curVideoMode);

			if (curVideoMode == *selectedVideoMode)
				selectedVideoModeIdx = i;
		}

		// Create the list box
		GUIListBox* videoModeListBox = GUIListBox::create(videoModeLabels);
		rightLayout->addElement(videoModeListBox);

		// Select the default (desktop) video mode
		videoModeListBox->selectElement(selectedVideoModeIdx);

		// Set up a callback to be notified when video mode changes
		videoModeListBox->onSelectionChanged.connect(&videoModeChanged);
	}

	void toggleFullscreen()
	{
		RenderWindowPtr window = gApplication().getPrimaryWindow();

		// In order to toggle between full-screen and windowed mode we need to use a CoreAccessor.
		// Banshee is a multi-threaded engine and when you need to communicate between simulation and
		// core thread you will use a CoreAccessor. Calling a core accessor method will essentially
		// queue the method to be executed later. Since RenderWindow is a core object you need to use
		// CoreAccessor to modify and access it from simulation thread, except where noted otherwise.

		// Classes where it is not clear if they are to be used on the core or simulation thread have
		// it noted in their documentation. e.g. RenderWindow::setWindowed method is marked as "Core only".
		// Additional asserts are normally in place for debug builds which make it harder for you to accidentally
		// call something from the wrong thread.
		if (fullscreen)
		{
			window->setWindowed(gCoreAccessor(), windowResWidth, windowResHeight);
		}
		else
		{
			window->setFullscreen(gCoreAccessor(), *selectedVideoMode);
		}

		fullscreen = !fullscreen;
	}

	void renderWindowResized()
	{
		RenderWindowPtr window = gApplication().getPrimaryWindow();
		const RenderWindowProperties& rwProps = window->getProperties();

		if (!fullscreen)
		{
			windowResWidth = rwProps.getWidth();
			windowResHeight = rwProps.getHeight();
		}

		sceneCamera->setAspectRatio(rwProps.getWidth() / (float)rwProps.getHeight());
	}

	void videoModeChanged(UINT32 idx)
	{
		selectedVideoMode = videoModes[idx];

		if (fullscreen)
		{
			RenderWindowPtr window = gApplication().getPrimaryWindow();
			window->setFullscreen(gCoreAccessor(), *selectedVideoMode);
		}
	}

	void buttonUp(const VirtualButton& button, UINT32 deviceIdx)
	{
		// Check if the pressed button is one of the either buttons we defined
		// in "setUpExample", and toggle profiler overlays accordingly.
		// Device index is ignored for now, as it is assumed the user is using a single keyboard,
		// but if you wanted support for multiple gamepads you would check deviceIdx.
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
}

