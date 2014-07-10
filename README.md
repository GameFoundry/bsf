# BansheeEngine

A free & modern multi-platform game development toolkit. It aims to provide simple yet powerful environment for creating games and other graphical applications. A wide range of features are available, ranging from a math and utility library, to DirectX 11 and OpenGL render systems all the way to asset processing, fully featured editor and C# scripting.

Banshee aims to become a standard in game development by providing rock solid implementations of known and widely used techniques while also being flexible enough to implement constantly changing cutting edge techniques. Built from ground up to be a modern library using modern design principles - everything is modular, layered and decoupled as much as possible, making it easier to modify, replace or add functionality. Platform specific functionality is kept at a minimum making porting as easy as possible. Additionally every non-trivial method, class and field is documented.

On top of the powerful and flexible C++ core lies a fully featured editor and a C# scripting system. C# scripting makes your development easier by giving you access to the entire .NET library, along with fast iterations times while being safe and easy to write. Editor is fully extensible with the help of C# scripting so you may customize it for exact needs of your project.

## Download/Install

To get Banshee to run you will need to check out the source code from GitHub and additionally you will need dependencies only available for download below. Dependencies should be extracted in the same folder as the root folder of the project (they share folder structure so it should be easy to see what goes where).

To compile Banshee you will need Visual Studio 2013 (Express version will work, but earlier Visual Studio versions will not). Other Windows compilers might work but have not been tested. Support for more platforms and compilers will become available with time.

[Download dependencies] (http://bearishsun.thalassa.feralhosting.com/BansheeDependencies.rar)

To compile DirectX render systems you will also need a separately installed DirectX SDK. Check "Dependencies.txt" for all information regarding used dependencies.

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
* Powerful GUI system
  * Unicode text rendering and input
  * Easy to use layout based system
  * Many common GUI controls
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

Easiest way to get started with Banshee is to check out the `ExampleProject` included with the source code. However to give you a taste here are a few code snippets.

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
  HMesh dragonModel = Importer::instance().import<Mesh>("Dragon.fbx");
  HTexture dragonTexture = Importer::instance().import<Texture>("Dragon.psd");
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

Banshee is developed by Marko Pintera. I built the entire project from in my free time out of personal interest in game engine development, never having had the chance to do it professionally. As time went by it evolved into something more and it's now hard to believe how far the project has progressed since I started. With time (especially if community joins in) I hope we can build something that can rival true AAA projects.

I'd love to hear input from other developers, especially if it's positive! Contact me at marko.pintera@com.gmail (antispam: flip gmail/com). 
