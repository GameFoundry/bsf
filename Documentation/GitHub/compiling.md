# Compiling

Supported compilers:
 - MSVC++ 14.0 (Visual Studio 2015)
 - Clang

Supported platforms:
 - Windows
 - Mac/Linux COMING SOON
 
VS2015 solution is provided by default, as well as a CMake lists file which can be used for generating non-VS builds.

# Third party dependencies
Aside from Banshee source code you will also need various third party dependencies. You may retrieve/compile those dependencies yourself by following a guide in "CompilingDependenciesManually.txt" (provided with the source code). 

Optionally, if you are using VS2015 you can avoid compiling dependencies by downloading the archive below, containing a set of pre-compiled dependencies. These should be extracted in the root of the directory containing Banshee source code.

[Download dependencies (VS2015)] (http://bearishsun.thalassa.feralhosting.com/BansheeDependencies_VS2015_v0.3.0.zip)

# Data files
You will also need data files (non-code resources like images, shaders, etc.) in order to run the editor, engine and example projects.

[Download data files] (http://bearishsun.thalassa.feralhosting.com/BansheeData_v0.3.0.zip)