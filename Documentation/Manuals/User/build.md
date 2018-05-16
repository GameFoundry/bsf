Setting up a project				{#build}
===============

This manual will explain how to set up your own project and link it with bs::framework.

# Get `bsf` binaries

The first step is to actually get `bsf`. You have two options:
 1. Get the precompiled set of binaries from the [Download](https://www.bsframework.io/download.html) page. This is the simplest and easiest solution.
 2. Grab the source code and compile the program yourself. You can either get the latest code from the [GitHub](https://github.com/GameFoundry/bsf) repository, or download an archived version of one of the releases from the [Download](https://www.bsframework.io/download.html) page. Follow the compilation guide on the GitHub page for detailed instructions.
 
# Create your project

Whether you downloaded precompiled binaries or compiled them yourself you should now have three folders containing the entirety of `bsf`:
 - `bin` - Contains dynamic libraries and data files
 - `include` - Contains public header files
 - `lib` - Contains import libraries (Windows only)

In order to get your project running you will need to:
 - Provide a path to the bsf's include folder to your compiler
 - Link with the `bsf` dynamic/shared library
 - Make sure your executable can find all the dynamic libraries and data files

How to perform these steps depends on your build tool of your choice. We will explore two options:
 - **CMake** - Create a CMake project that will ultimately work with any major build tool
 - **Visual Studio** - Create a VS solution from scratch
 
## CMake
CMake is a build system that allows you define a project that can then be used for building across multiple platforms and build tools. Such CMake project can be used to create a Visual Studio solution, XCode project or Unix Makefiles. `bsf` also comes with modules that make using `bsf` in your CMake project easier.

Note that CMake is fairly complex and we don't go into details about all of its syntax. There are many tutorials that do that. However our project will be pretty simple and should be easy to understand even to a newcomer to CMake.

### Preparation
First create a new directory where you would like your project to live. Copy the `Source/CMake/Modules/Findbsf.cmake` to the root of your project folder. This file will be used for helping us find `bsf` libraries and includes.

### CMake project
In your project folder create `CMakeLists.txt`, and fill it out like so:
```
# Name of your project
project (myProject)

# Make sure to use the C++14 standard
set(CMAKE_CXX_STANDARD 14)

# Build an executable from the provided C++ files
add_executable(myApp "Main.cpp")

# Path to where you have installed bsf
set(bsf_INSTALL_DIR "C:/path/to/bsf/install")

# Let CMake know where to find the Findbsf.cmake file (at current folder)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# Find bsf libraries and headers
find_package(bsf REQUIRED)

# Link bsf library with your application
target_link_libraries(myApp PRIVATE bsf)
```

### Code
The CMake file above references `Main.cpp` file. The most basic file that runs the framework looks like so:
```
#include "BsApplication.h"

int main()
{
	using namespace bs;

	VideoMode videoMode(1280, 720);
	Application::startUp(videoMode, "Example", false);
	Application::instance().runMainLoop();
	Application::shutDown();

	return 0;
}
```

We will explain the code above in following manuals, but for now you can just copy/paste it and use it for the starting point.

### Get CMake
If you don't already have it, grab the latest version of CMake from [www.cmake.org](https://cmake.org/download/). Make sure to grab version 3.9.0 or later. 

### Build
You are now ready to build the project. From your project's root folder execute the following commands in terminal/command line:
 - `mkdir build`
 - `cd build`
 - `cmake -G "$generator" ..`
   - Where *$generator$* should be replaced with any of the supported generators. Some common ones:
     - `Visual Studio 15 2017 Win64` - Visual Studio 2017 (64-bit build)
	 - `Unix Makefiles`
	 - `Ninja`
	 - `Xcode`
	 - See all valid generators: [cmake-generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)
   - If your system complains that `cmake` is not a valid command, you need to add the path of CMake binaries to your $PATH environment variable
	 
Your build files will be output to the `build` folder. 
 - If you are using Visual Studio open up the `build/bsf.sln` solution and proceed building as normal
 - If you are using XCode open up the project in the `build` folder and proceed building as normal
 - If you are using Makefiles simply run `make` within the `build` folder
 
### Copy files
Before you can run the application you must first copy the data files as well as dynamic libraries. You should copy the entire contents of the bsf's `bin` folder to the location where is your executable located. For example in Visual Studio this is by default a path similar to `x64/Release/` within your project root.

![Structure of the executable folder on Windows](BuildStructure.png)

### Run
You are now ready to run the application. If you used the default `Main.cpp` code above an empty window should open.



TODO - VS Project
TODO - Source code project