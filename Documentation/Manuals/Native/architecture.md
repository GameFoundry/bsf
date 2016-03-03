Architecture									{#architecture}
===============
[TOC]

Banshee is implemented throughout many separate libraries. Spreading the engine implementation over different libraries ensures multiple things:
 - Portions of the engine can be easily modified or replaced
 - User can choose which portions of the engine are needed
 - Internals are easier to understand as libraries form a clear architecture between themselves, while ensuring source code isn't all bulked into one big package
 - It insures quality design by decoupling code and forcing an order of dependencies.
 
All the libraries can be separated into four main categories:
 - Layers - These are the core libraries of the engine. Each layer is built on top of the previous layer and provides higher level and more specific functionality than the other.
 - Plugins - These are separate, independant, and in most cases optional libraries containing various high level systems. They always depend on a specific layer, depending how high level the system they implement is. You will be able to design your own plugins that completely replace certain portion of the engine functionality without having to modify the engine itself (e.g. use OpenGL instead of DirectX)
 - Scripting - These are libraries used for interop between the C++ core and the scripting language, as well as the managed scripting libraries themselves. Generally these can also be counted as plugins, but they are a bit more tightly coupled with the engine core.
 - Executable - These are small pieces of code meant to initialize and start up the engine/editor.
 
# Layers #										{#arch_layers}
The layers contain the core of the engine. All the essentials and all the abstract interfaces for plugins belong here. The engine core was split into multiple layers for two reasons:
 - To give developers a chance to pick the level of functionality they need. Some people will want just core and utility and start working on their own engine while others might be just interested in game development and will stick with the editor layer.
 - To decouple code. Lower layers do not know about higher levels and low level code never caters to specialized high level code. This makes the design cleaner and forces a certain direction for dependencies.

Lower levels were designed to be more general purpose than higher levels. They provide very general techniques usually usable in various situations, and they attempt to cater to everyone. On the other hand higher levels provide a lot more focused and specialized techniques. This might mean relying on very specific rendering APIs, platforms or plugins but it also means using newer, fancier and maybe not as widely accepted techniques (e.g. some new rendering algorithm or a shader).

Going from the lowest to highest the layers are:
## BansheeUtility ##							{#arch_utility}
This is the lowest layer of the engine. It is a collection of very decoupled and separate systems that are likely to be used throughout all of the higher layers. Essentially a collection of tools that are in no way tied into a larger whole. Most of the functionality isn't even game engine specific, like providing file-system access, file path parsing or events. Other things that belong here are the math library, object serialization and RTTI system, threading primitives and managers, among various others.

## BansheeCore ##								{#arch_core}
It is the second lowest layer and the first layer that starts to take shape of an actual engine. This layer provides some very game-specific modules tied into a coherent whole, but it tries to be very generic and offer something that every engine might need instead of focusing on very specialized techniques. Render API wrappers exist here, but actual render APIs are implemented as plugins so you are not constrained by specific subset. Scene manager, renderer, resource management, audio, animation, physics, importers and others all belong here, and all are implemented in an abstract way that they can be implemented/extended by higher layers or plugins.

## BansheeEngine ##								{#arch_engine}
Second highest layer and first layer with a more focused goal. It is built upon BansheeCore but relies on a specific sub-set of plugins and implements systems like scene manager and renderer in a specific way. For example DirectX 11 and OpenGL render systems are referenced by name, as well as Mono scripting system among others. Renderer that follows a specific set of techniques and algorithms that determines how are all objects rendered also belongs here.

## BansheeEditor ##								{#arch_editor}
And finally the top layer is the editor. It provides various editor specific features like the project library, built systems, editor window systems, scene view tools and similar. Large portions of the editor are implemented in the scripting code, and this layer provides more of a set of helper tools used by the scripting system. If you are going to work with this layer you will also be working closely with the scripting interop code and the scripting code.

# Plugins #										{#arch_plugins}
Banshee provides a wide variety of plugins out of the box. Some of these are fully optional, but all can be replaced with your own implementations. Each plugin is based on an abstract interface implemented in one of the layers (for the most part, BansheeCore and BansheeEngine layers).

## Render API ##								{#arch_rapi}		
Render API plugins allow you to use a different backend for performing hardware accelerated rendering. Render API handles low level rendering, including features like vertex/index buffers, creating rasterizer/depth/blend states, shader programs, render targets, textures, draw calls and similar. Render API interface is defined in the BansheeCore layer.

A few out of the box implementations are provided, the user can choose which one to use upon application startup. At least one must be present during start-up (can be a null renderer (i.e. one that does nothing, like for server purposes) if you choose to add one).

### BansheeD3D11RenderAPI ###					{#arch_dx11rapi}
Provides implementation of the Render API interface for DirectX 11. 

### BansheeD3D9RenderAPI ###					{#arch_dx9rapi}
Provides implementation of the RenderAPI interface for DirectX 9. 

### BansheeGLRenderAPI ###						{#arch_GLrapi}
Provides implementation of the RenderAPI interface for OpenGL 4.3.

## Importers ##									{#arch_importers}		
Importers allow you to convert various types of files into formats easily readable by the engine. Normally importers are only used during development (e.g. in the editor), but the game itself will only use previously imported assets (although ultimately that's up to the user).

All importers implement a relatively simple interface represented by a single SpecificImporter class. The engine can start with zero importers, or with as many as you need.

See [this link](TODOLINK) on a guide how to add your own importer. Some important importers are provided out of the box:

### BansheeFreeImgImporter ###					{#arch_freeimg}	
Handles import of most popular image formats, like .png, .psd, .jpg, .bmp and similar. It uses the FreeImage library for reading the image files and converting them into engine Texture format.

### BansheeFBXImporter ###						{#arch_fbx}
Handles import of FBX mesh files. Uses Autodesk FBX SDK for reading the files and converting them into engine Mesh format.

### BansheeFontImporter ###						{#arch_font}
Handles import of TTF and OTF font files. Uses FreeType for reading the font files and converting them into engine Font format.

### BansheeSL ###								{#arch_bsl}	
Provides an implementation of the Banshee's shader language that allows you to easily define an entire pipeline state in a single file.

## Others ##									{#arch_others}

### BansheeOISInput ###							{#arch_ois}		
Handles raw mouse/keyboard/gamepad input for multiple platforms. Implements the RawInputHandler interface. Uses the OIS library specifically modified for Banshee (source code available with Banshee's dependencies). Be aware that there is also an OSInputHandler that is used for non-game specific purposes (e.g. tracking cursor, text input), but that is part of the engine core instead of a plugin.

### BansheePhysX ###							{#arch_physx}
Handles physics: rigidbodies, colliders, triggers, joints, character controller and similar. Implements the Physics interface (and all related classes). Uses NVIDIA PhysX as the backend.

### BansheeMono ###								{#arch_mono}
Provides access to the C# scripting language using the Mono runtime. All the script interop libraries (listed below) depend on this plugin, as well as a specific engine layer.

### RenderBeast ###								{#arch_rbeast}	
Banshee's default renderer. Implements the Renderer interface. This plugin might seem similar to the Render API plugins mentioned above but it is a higher level system. While Render API plugins provide low level access to rendering functionality the renderer handles rendering of all scene objects in a specific manner without having to issue draw calls manually. A specific set of options can be configured, both globally and per object that control how an object is rendered, as well as specifying completely custom materials. e.g. the renderer will handle physically based rendering, HDR, shadows, global illumination and similar features.

See [this link](TODOLINK) for more information on how the renderer works and how to potentially implement your own.

# Scripting #									{#arch_scripting}
Scripting libraries can be placed into two different categories: 
 - Interop libraries written in C++. These provide glue code between C++ and C# (e.g. converting types, managing objects and similar). These are prefixed with the letter S.
 - Managed libraries containing actual managed code. These are prefixed with the letter M.
 
Whenever adding a new type that exists in both script and managed code you will need to access both of these library categories. Read [this guide](TODOLINK) to learn more about adding your own scripting types.
 
## SBansheeEngine ##							{#arch_sengine}
Interop library that provides glue code between BansheeEngine layer and the scripting code in MBansheeEngine.

## SBansheeEditor ##							{#arch_seditor}	
Interop library that provides glue code between BansheeEditor layer and the scripting code in MBansheeEditor.

## MBansheeEngine ##							{#arch_mengine}
Provides most of the high-level C++ engine API available in BansheeEngine layer (e.g. scene object model, resources, GUI, materials, etc.) as a high-level C# scripting API. Certain portions of the API are provided in pure C# (like the math library).

## MBansheeEditor ##							{#arch_meditor}
Provides all editor specific functionality from the BansheeEditor layer as a C# scripting API (e.g. project library, building, scene view, etc.). Also implements a large number of editor features in pure C# on top of the provided engine and editor APIs (e.g. specific editor windows and tools).