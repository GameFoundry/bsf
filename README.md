# BansheeEngine

A free & modern multi-platform game development toolkit. It aims to provide simple yet powerful environment for creating games and other graphical applications. A wide range of features are available, ranging from a math and utility library, to DirectX 11 and OpenGL render systems all the way to asset processing, fully featured editor and C# scripting.

Banshee aims to become a standard in game development by providing rock solid implementations of known and widely used techniques while also being flexible enough to implement constantly changing cutting edge techniques. Built from ground up to be a modern library using modern design principles - everything is modular, layered and decoupled as much as possible, making it easier to modify, replace or add functionality. Platform specific functionality is kept at a minimum making porting as easy as possible. Additionally every non-trivial method, class and field is documented.

On top of the powerful and flexible C++ core lies a fully featured editor and a C# scripting system. C# scripting makes your development easier by giving you access to the entire .NET library, along with fast iterations times while being safe and easy to write. Editor is fully extensible with the help of C# scripting so you may customize it for exact needs of your project.

## Download/Install

To get Banshee to run you will need to check out the source code from GitHub and additionally you will need dependencies available for download below. Dependencies should be extracted in the same folder as the root folder of the project (they share folder structure so it should be easy to see what goes where).

To compile Banshee you will need Visual Studio 2013 (Express version will work, but earlier Visual Studio versions will not). Other Windows compilers might work but have not been tested. Support for more platforms and compilers will become available with time.

[Download dependencies] (http://bearishsun.thalassa.feralhosting.com/BansheeDependencies.rar)

To compile DirectX render systems you will also need a separately installed DirectX SDK. Check "Dependencies.txt" for all information regarding used dependencies.

## Features (currently available)

* Editor
  * Asset management
    * Simple drag and drop import
	* Automatic reimport of externally modified assets
	* Asset modifications immediately reflected in-game
    * Version control friendly format
  * Powerful object inspector
    * Automatic GUI for script objects
    * Exposes properties for artists/designers
    * Customizable per-type
  * Level creation
    * Simple drag and drop interface
	* Traditional set of tools
    * Custom 2D and 3D tool support
  * Prefab system
    * Pre-built templates for level design
	* Easy to create and maintain complex levels
	* Hierarchical prefabs and instance specialization
  * Play in editor
    * Compile in editor
    * Immediately test changes
	* Pause and frame-step
	* Analyze and modify scene while playing
  * Fully extensible for game-specific needs
    * Easy to extend using scripting
    * Comprehensive extension scripting API
	* Extend almost anything
	  * Editor windows
	  * Object inspectors
	  * 2D/3D tools
	  * Automate common tasks
  * Game publishing
    * One click build process
	* Automatically detect required resources
	* Automatically package and output an executable
  * Customizable frontend
    * Dockable layout and floating windows
	* Custom skin & localization support
   
* Core
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
	  * BansheeFX language for material definitions
      * Shader parsing for HLSL9, HLSL11 and GLSL
  * Asset pipeline
    * Easy to use
    * Asynchronous resource loading
    * Extensible importer system
    * Available importer plugins for:
      * FBX, OBJ, DAE meshes
      * PNG, PSD, BMP, JPG, ... images
      * OTF, TTF fonts
      * HLSL9, HLSL11, GLSL shaders
  * Powerful GUI system
    * Unicode text rendering and input
    * Easy to use layout based system
    * Many common GUI controls
    * Fully skinnable
    * Automatic batching
    * Support for texture atlases
    * Localization
  * Scripting
    * C# via Mono
	* Separate high level engine API
	* Integrated runtime for maximum performance
    * Full access to .NET framework
	* Integration with Visual Studio
	* Automatic serialization
	  * Custom components
	  * Custom resources
	  * No additional code
	  * Handles complex types and references
  * Other
    * CPU & GPU profiler
    * Virtual input
    * Advanced RTTI system
    * Automatic object serialization/deserialization
    * Debug drawing
    * Utility library
      * Math, file system, events, thread pool, task scheduler, logging, memory allocators and more
    
## Features (upcoming)
 * DirectX 12/Vulkan support
 * High quality renderer
 * Physics system integration
 * Audio system integration
 * Video system integration
 * Networking system integration
 * Animation
 * Mac & Linux support

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

Banshee is offered completely free for personal or commercial use under the General Public License v3 (GPL v3). Read `BansheeLicense.txt` included in the License sub-directory for full license text. Licenses and credits for used third-party libraries and tools are included in the same directory.

# Author

Banshee is developed by Marko Pintera. I've been a professional game developer for the last five years working on various mid-sized titles. My interests lie in engine and graphics development which I spend most of my free time on.

Contact me at marko.pintera@com.gmail (antispam: flip gmail/com). 