# BansheeEngine

A free & modern multi-platform game development toolkit. It aims to provide simple yet powerful environment for creating games and other graphical applications. A wide range of features are offered, ranging from a math and utility library, to DirectX 11 and OpenGL render systems all the way to asset processing, fully featured editor and C# scripting.

## Download

Along with the source you will need these dependencies (Extract to the same folder as the source code):
[Download dependencies] (http://nolinkyet)

Or download the combined package:
[Download source + dependencies] (http://nolinkyet)

## Features (currently available)

* Design
  * Built using C++11 and modern design principles
  * Clean layered design
  * Fully documented
  * Modular & plugin based
  * Multiplatform ready
* Renderer
  * DX9, DX11 and OpenGL 4.3 render systems
  * Multi-threaded rendering
  * Flexible material system
    * Easy to control and set up
    * Shader parsing for HLSL9, HLSL11 and GLSL
* Asset pipeline
  * Easy to use
  * Asynchronous resource loading
  * Extensible importer system
  * Available importer plugins for:
    * FXB,OBJ, DAE meshes
    * PNG, PSD, BMP, JPG, ... images
    * OTF, TTF fonts
    * HLSL9, HLSL11, GLSL shaders
Powerful GUI system
  * Unicode text rendering and input
  * Easy to use layout based system
  * Most common GUI controls
  * Fully skinnable
  * Automatch batching
  * Support for texture atlases
  * Localization
* Other
  * CPU & GPU profiler
  * Virtual input
  * Advanced RTTI system
  * Automatic object serialization/deserialization
  * Debug drawing
  * Utility library
    * Math, file system, events, thread pool, task scheduler, logging, memory allocators and more
    
## Features (upcoming)

 * C# scripting support (in development, coming soon)
 * Multi-purpose and extensible editor (in development, coming soon)
 * High quality renderer
 * Physics system integration
 * Audio system integration
 * Video system integration
 * Networking system integration
 * Animation
 * GUI animation

## Development state

Project is currently in active development. Current version is considered a preview version. Bugs are to be expected and new features will be added as development progresses.

## Jump in

Easiest way to get started with Banshee is to check out `ExampleProject` included with the source code. However to give you a taste here are a few code snippets.

### Starting a minimal application
```
	RENDER_WINDOW_DESC renderWindowDesc;
	renderWindowDesc.videoMode = VideoMode(1280, 720);
	renderWindowDesc.title = "My App";
	renderWindowDesc.fullscreen = false;
	
	Application::startUp(renderWindowDesc, RenderSystemPlugin::DX11);
	Application::instance().runMainLoop();
	Application::shutDown();
```

### Importing resources
```
  HMesh dragonModel = static_resource_cast<Mesh>(Importer::instance().import("Dragon.fbx"));
  HTexture dragonTexture = static_resource_cast<Texture>(Importer::instance().import("Dragon.psd"));
```

### Adding and positioning a camera
```
  HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
  HCamera sceneCamera = sceneCameraSO->addComponent<Camera>(window);

	sceneCameraSO->setPosition(Vector3(40.0f, 30.0f, 230.0f));
	sceneCameraSO->lookAt(Vector3(0, 0, 0));
```

### Adding an object for rendering
```
  HSceneObject dragonSO = SceneObject::create("Dragon");
  
	HRenderable renderable = dragonSO->addComponent<Renderable>();
	renderable->setMesh(dragonModel);
	renderable->setMaterial(dragonMaterial);
```

### Adding GUI
```
  HSceneObject guiSO = SceneObject::create("GUI");
  HCamera guiCamera = guiSO->addComponent<Camera>(window);
  
  HGUIWidget gui = guiSO->addComponent<GUIWidget>(guiCamera->getViewport().get());
  GUIArea* guiArea = GUIArea::createStretchedXY(*gui, 0, 0, 0, 0);
  guiArea->getLayout().addElement(GUIButton::create(HString(L"Click me!")));
  guiArea->getLayout().addElement(GUIButton::create(HString(L"Click me too!")));
```

# License

Banshee is offered completely free for personal or commercial use. Only requirement is that you include Banshee Logo in your application when using any part of Banshee. Read `BansheeLicense.rtf` included with the project for more details. 

# Author

Banshee is developed by Marko Pintera. I created and built the entire project from my bedroom in my free time out of personal interest in game engine development, never having had the chance to do it professionally. 

I appreciate any feedback at marko.pintera@com.gmail (antispam: flip gmail/com).
