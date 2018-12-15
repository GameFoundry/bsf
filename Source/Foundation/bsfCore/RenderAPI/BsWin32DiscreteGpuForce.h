//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//

/**
 * This Header file is used on Windows-based systems to force the operating system
 * to use discrete graphics chips on laptops instead of the integrated graphics chips.
 * @note This header shouldn't be used on non-Windows based operating systems and
 *        and it should only be included by final executables.
 *        Incorrect usage will cause error during compilation.
 */
 
#if BS_PLATFORM == BS_PLATFORM_WIN32
#if defined(BS_FORCE_DISCRETE_GPUS_INCLUDE_CHECK)
	#error GPU force header file already included. Please, only include it once and preferably in the program's executable. Aborting compilation.
#else
#define BS_FORCE_DISCRETE_GPUS_INCLUDE_CHECK
 
extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#endif
 
#endif 