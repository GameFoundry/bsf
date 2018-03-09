Getting started								{#gettingStarted}
===============
[TOC]

This manual offers a quick overview of commonly used bs::f functionality, in order to give you a better idea of how bs::f works. For a fully working example check out the `ExampleGettingStarted` project available with the source code.

# Starting an application
bs::f is started through the @ref bs::Application "Application" interface. To start the engine you need to provide it with a description of the primary render window.

The application is started by calling @ref bs::Application::startUp "Application::startUp()", after which you can set up your custom code in the form of components (see later). Finally you can run the main loop with @ref bs::Application::runMainLoop "Application::runMainLoop()" which will execute your code and actually get everything in motion.

Once the main loop terminates use @ref bs::Application::shutDown "Application::shutDown()" to clean everything up.

~~~~~~~~~~~~~{.cpp}
// Start an application in windowed mode using 1280x720 resolution
Application::startUp(
	VideoMode(1280, 720), // Window resolution
	"My app", // Window title
	false); // True for fullscreen, false for windowed

// Set up your scene here

Application::instance().runMainLoop();
Application::shutDown();
~~~~~~~~~~~~~

# Importing resources
To import a resource from a third party format into an engine-readable format use the @ref bs::Importer "Importer" module. A variety of formats like PNG, JPG, PSD, FBX, etc. are supported. Read the @ref resourceBasicsAndImport manual for more information about resources and import. 

In the example below we'll import a @ref bs::Mesh "Mesh" and a @ref bs::Texture "Texture".
~~~~~~~~~~~~~{.cpp}
HMesh dragonModel = gImporter().import<Mesh>("Dragon.fbx");
HTexture dragonTexture = gImporter().import<Texture>("Dragon.psd");
~~~~~~~~~~~~~

# Setting up a material
Once we have a mesh and a texture we need some way to apply that texture to the mesh. For that reason we first import a @ref bs::Shader "Shader" that describes how is an object rendered, which we then use to create a @ref bs::Material "Material" which allows us to apply our previously loaded **Texture**.

bs::f uses .bsl files to describe shaders and you can learn more about BSL syntax in the @ref shaders manual. To learn more about materials and how to use them read the @ref simpleMaterial manual.

~~~~~~~~~~~~~{.cpp}
HShader diffuse = gImporter().import<Shader>("Diffuse.bsl");
HMaterial dragonMaterial = Material::create(diffuse);

dragonMaterial->setTexture("albedo", dragonTexture);
~~~~~~~~~~~~~

# Adding an object for rendering
To actually make our mesh render with our material, we need to add a @ref bs::SceneObject "SceneObject" onto which we attach a @ref bs::CRenderable "CRenderable" component. This component allows us to set up a mesh and a material, after which they will be automatically rendered to the active camera.

To learn more about scene objects and components read the @ref scenesAndComponents manual. You can use the same approach we followed here to add your own custom components, containing custom code - read more about custom components in the @ref customComponents manual.

You can also read the @ref renderingObjects manual to learn more about how the **CRenderable** component works.

~~~~~~~~~~~~~{.cpp}
HSceneObject dragonSO = SceneObject::create("Dragon");

HRenderable renderable = dragonSO->addComponent<CRenderable>();
renderable->setMesh(dragonModel);
renderable->setMaterial(dragonMaterial);
~~~~~~~~~~~~~

# Adding a scene camera
In order to actually see our object we need to set up a camera. First create a new **SceneObject** onto which you can then attach a @ref bs::CCamera "CCamera" component. After that we position the **SceneObject** so it is looking at the object we set up in previous steps. Camera required an output surface, for which we use the primary application window as retrieved from @ref bs::Application::getPrimaryWindow() "Application::getPrimaryWindow()". Learn more about cameras in the @ref cameras manual.

~~~~~~~~~~~~~{.cpp}
SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
HCamera sceneCamera = sceneCameraSO->addComponent<CCamera>(window);

sceneCameraSO->setPosition(Vector3(40.0f, 30.0f, 230.0f));
sceneCameraSO->lookAt(Vector3(0, 0, 0));
~~~~~~~~~~~~~

# Adding a GUI element
Finally you might want to add a GUI to your application. Similar to above, create a new **SceneObject** onto which we add a new **CCamera** used only for rendering GUI. Since we don't want it to render normal scene objects, we let it's filter `layers` to 0. 

We also add a @ref bs::CGUIWidget "CGUIWidget" component onto the same **SceneObject**, which is used as a container for all GUI elements. After that we add a couple of @ref bs::GUIButton "GUIButton"%s to the GUI.

Read the @ref guiElements and related manuals for more information about GUI.

~~~~~~~~~~~~~{.cpp}
HSceneObject guiSO = SceneObject::create("GUI");

HCamera guiCamera = guiSO->addComponent<CCamera>(window);
guiCamera->setLayers(0); // Force camera to ignore normal scene geometry

HGUIWidget gui = guiSO->addComponent<CGUIWidget>(guiCamera);
GUIPanel* guiPanel = gui->getPanel();

GUILayout* guiLayout = guiPanel->addNewElement<GUILayoutY>();
guiLayout->addNewElement<GUIButton>(HString(L"Click me!"));
guiLayout->addNewElement<GUIButton>(HString(L"Click me too!"));
~~~~~~~~~~~~~

# Final result
@ref TODO_IMAGE

There is a lot more to bs::f, but hopefully this gave you a quick taste of how it works. Continue reading other manuals and the API reference for more information.