# Features

Here you will find a complete list of all currently available features. Banshee is current in development and more features are being added frequently.

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