# Compiling

## Quick start
Use the commands below to get Banshee up and running as quickly as possible. Scroll further below for advanced build options that allow for more customization.

**Windows (Visual Studio 2015)**

Make sure you have installed *git* and *CMake* (version 3.7.2 or newer). Make sure paths to their binaries are in your *PATH* enviroment variable.

Run the following commands and the *.sln* will be placed in the */Build* folder:
```
git clone https://github.com/BearishSun/BansheeEngine.git
cd BansheeEngine
mkdir Build
cd Build
cmake -G "Visual Studio 14 2015 Win64" ../Source
```

>Note: If compilation fails with errors related to DirectX, you need to install relevant DirectX dependencies as described [here](#otherDeps).

**Linux**

*Coming soon*

**OS X**

*Coming soon*

## Customizing the build

By default the process above will fetch the *master* branch. If required switch to a different branch before building:
 - **preview** - This branch always contains latest "stable" pre-release version. This might be a few months behind master.
 - **master** - This branch contains the latest changes, might be quite unstable and might not even compile. Use this if you absolutely need the bleeding edge changes.
 
Variety of *CMake* variables are provided that allow you to customize the build:
 
- By default the process above will build the complete Banshee 3D runtime. You can pick between two modes using the *BUILD_SCOPE* variable in *CMake*:
  - **Runtime** (default) - This will compile the entire engine, scripting system and the editor. This is intended for those that plan on using the editor as their main environment for development.
  - **Framework** - This will compile only the C++ engine framework without the editor or the scripting API. This is intended for those that prefer to develop more directly with less high level abstractions.
- Additional variables allow you to pick between the render API (Vulkan, DirectX, OpenGL), audio module (FMOD, OpenAudio) among other options. Run *CMake* to see all options.
- Note that non-default *CMake* options might require additional dependencies to be installed, see [here](#otherDeps).
 
You can choose to use a different *CMake* generator than those specified above, as long as the platform/compiler is supported:  
  - Supported platforms:
    - Windows
    - (Mac & Linux coming soon)
  - Supported compilers:
    - MSVC++ 14.0 (Visual Studio 2015)
    - (Clang/GCC coming soon)
 
### <a name="dependencies"></a>Third party dependencies
Banshee relies on a variety of third party dependencies. A set of pre-compiled dependencies are provided for every supported platform/compiler. These will be fetched automatically by the build process, but you may also download them manually or compile them from source:
 * [Download dependencies (VS2015)](http://data.banshee3d.com/BansheeDependencies_VS2015_Master.zip)
 * [Compiling dependencies manually](dependencies.md)
 
For older versions of pre-compiled dependencies check the git release tag descriptions for links.

## <a name="otherDeps"></a>Other dependencies
The following dependencies will need to be installed manually. Which ones are required depend on the selected *CMake* options and your usage of Banshee (check text of each entry below).

**DirectX SDK** (Optional if not using DirectX)
 - Only needed if on Windows 7 or earlier and using DirectX 11 render API
 - Set up DXSDK_DIR environment variable pointing to the DirectX instalation
 
**Windows SDK** (Optional if not using DirectX)
 - Only needed if on Windows 8 or later and using DirectX 11 render API
 
**DirectX Debug Layer** (Optional if not using DirectX)
 - Only needed if on Windows 10 and using DirectX 11 render API
 - Go to Settings panel (type "Settings" in Start)->System->Apps & features->Manage optional Features->Add a feature->Select "Graphics Tools"
 
**Vulkan SDK** (Optional if not using Vulkan) 
 - Only needed if you selected the Vulkan render API during build configuration
 - https://lunarg.com/vulkan-sdk/
 - If CMake complains it cannot find Vulkan, manually set the Vulkan_INSTALL_DIRS to your installation directory
 
**Python 3.5** (Optional)
 - Only needed if you plan on running Python scripts in the /Scripts folder
 - https://www.python.org/downloads/

**FMOD Low Level Programmer API** (Optional)
 - Only needed if you selected the FMOD audio module during build configuration
 - http://www.fmod.org/download/
 - If CMake complains it cannot find FMOD, manually set the FMOD_INSTALL_DIRS to your installation directory 
 - Copy the dynamic libraries from {INSTALLDIR}/api/lowlevel/lib into /bin folder in Banshee source code folder
  - Use logging libraries for the Debug builds, and non-logging for OptimizedDebug and Release builds
  
**Mono 4.2** (Optional)
 - If you wish to compile managed assemblies using a Microsoft compiler (e.g. using Visual Studio) yet still be able to debug the generated assemblies, you must install Mono 4.2. and set up an environment variable MONO_INSTALL_DIR pointing to the Mono installation directory. When this is set up "pdb2mdb" script will trigger on next compile generating the needed debug symbols.
 - http://www.mono-project.com/download/
