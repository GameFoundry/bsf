# BansheeEngine

A modern open-source game development toolkit. It aims to provide simple yet powerful environment for creating games and other graphical applications. A wide range of features are available, ranging from a math and utility library, to DirectX 11 and OpenGL render systems all the way to asset processing, fully featured editor and C# scripting.

For game developers Banshee aims to provide a complete set of tools and APIs needed to make a game from start to finish. It provides a highly intuitive and customizable editor that is easy to use by artists, designers and programmers alike, supporting development stages from asset management, scene building, scripting to game publishing. C# scripting makes your development easier by giving you access to the entire .NET library, along with fast iterations times while being safe and easy to write. Editor is fully extensible with the help of specially developed scripting API so you may customize it for exact needs of your project.

For engine developers it aims to provide a high quality foundation to build and improve upon. Banshee runs on a powerful multi-threaded C++14 core created to take advantage of all resources modern hardware can offer. Built from ground up using modern design principles, everything is modular, layered and decoupled as much as possible, making it easier to modify, replace or add functionality. Platform specific functionality is kept at a minimum making porting as easy as possible. Additionally every non-trivial method, class and field is documented. All this combined makes Banshee a great platform to build upon, for developing higher level systems like editor tools, core enhancements like new rendering techniques and platform ports, all the way to creating fully specialized toolsets that fit your team's needs exactly.

![Banshee Editor](http://bearishsun.thalassa.feralhosting.com/BansheeEditor.png "Banshee Editor")

## Development state

Project is currently in active development. Current version is considered a preview version. Bugs are to be expected, optimization is not complete and some major features are yet to be added (see below for a list).

## Download/Install

Downloading pre-compiled binaries is the easiest way to check out Banshee if you don't want to go through the hassle of compiling it yourself. 

[Download binaries (Windows x64)] (http://bearishsun.thalassa.feralhosting.com/Banshee_Win_x64_v0.2.0.zip)

## Compiling from source

Banshee compiles on VS2013 and VS2015. Other Windows compilers might work but have not been tested. Support for more platforms and compilers will become available with time.

Aside from Banshee source code you will also need various third party dependencies. You may retrieve/compile those dependencies yourself by following a guide in "CompilingDependenciesManually.txt". If you are using VS2015 you can avoid compiling dependencies by downloading a set of pre-compiled dependencies below. These should be extracted in the root of the directory containing Banshee source code.

[Download precompiled dependencies (VS2015)] (http://bearishsun.thalassa.feralhosting.com/BansheeDependencies_VS2015_v0.2.0.zip)

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
    * Built using C++14 and modern design principles
    * Clean layered design
    * Fully documented
    * Modular & plugin based
	* Minimal third-party dependencies
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
  * GUI system
    * Unicode text rendering and input
    * Easy to use layout based system
    * Many common GUI controls
    * Fully skinnable
    * Automatic batching
    * Support for texture atlases
    * Localization
  * Scripting
    * C# 5.0
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
    * Native object serialization
    * Debug drawing
	* Crash reporting
    * Utility library
      * Math, file system, events, thread pool, task scheduler, logging, memory allocators and more
    
## Features (coming in near future)
 * DirectX 12/Vulkan support
 * Physically based renderer
 * Physics system integration
 * Audio system integration
 * Animation
 * Mac & Linux support

## Getting started

Banshee is a multi-layered engine that aims to be flexible enough to handle various needs. Therefore this section is split into two sub-sections, first one aimed for game developers (high-level C# programmers, artists, designers) and engine developers (low level C++ programmers).

### Getting started (Game developers)

TODO - Create a simple editor project with a mesh, texture and a basic script. Show the user how to load the project, add the objects to scene, attach the script, tweak material & script values and start playing the game. Provide a small tutorial on the C# scripting API as well (creating a scene object, custom component, handling input, moving the camera).

### Getting started (Engine developers)

Easiest way to get started with low-level Banshee programming is to check out the `ExampleProject` included with the source code. However to give you a taste here are a few code snippets.

#### Starting a minimal application
```
  RENDER_WINDOW_DESC renderWindowDesc;
  renderWindowDesc.videoMode = VideoMode(1280, 720);
  renderWindowDesc.title = "My App";
  renderWindowDesc.fullscreen = false;
	
  Application::startUp(renderWindowDesc, RenderSystemPlugin::DX11);
  Application::instance().runMainLoop();
  Application::shutDown();
```

#### Importing resources
```
  HMesh dragonModel = gImporter().import<Mesh>("Dragon.fbx");
  HTexture dragonTexture = gImporter().import<Texture>("Dragon.psd");
```

#### Adding and positioning a camera
```
  HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
  HCamera sceneCamera = sceneCameraSO->addComponent<CCamera>(window);

  sceneCameraSO->setPosition(Vector3(40.0f, 30.0f, 230.0f));
  sceneCameraSO->lookAt(Vector3(0, 0, 0));
```

#### Setting up a material
```
   HShader diffuse = gImporter().import<Shader>("Diffuse.bsl");
   HMaterial dragonMaterial = Material::create(diffuse);
   
   dragonMaterial->setTexture("albedo", dragonTexture);
```

#### Adding an object for rendering
```
  HSceneObject dragonSO = SceneObject::create("Dragon");
  
  HRenderable renderable = dragonSO->addComponent<CRenderable>();
  renderable->setMesh(dragonModel);
  renderable->setMaterial(dragonMaterial);
```

#### Adding GUI
```
  HSceneObject guiSO = SceneObject::create("GUI");
  HCamera guiCamera = guiSO->addComponent<CCamera>(window);
  HGUIWidget gui = guiSO->addComponent<CGUIWidget>(guiCamera);
  
  GUIPanel* guiPanel = gui->getPanel();
  GUILayout* guiLayout = guiPanel->addNewElement<GUILayoutY>();
  guiLayout->addNewElement<GUIButton>(HString(L"Click me!"));
  guiLayout->addNewElement<GUIButton>(HString(L"Click me too!"));
```

# License

Banshee is offered completely free for personal or commercial use under the GNU Lesser General Public License v3 (LGPL v3). A commercial license is also available for those that cannot comply with LGPL terms or just want more flexibility. 

# Author

Banshee was created and is developed by Marko Pintera.

Contact me at [e-mail] (http://scr.im/39d1) or add me on [LinkedIn] (https://goo.gl/t6pPPs). 