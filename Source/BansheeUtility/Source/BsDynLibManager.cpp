//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsDynLibManager.h"
#include "BsDynLib.h"

namespace BansheeEngine
{
	DynLibManager::DynLibManager()
	{
	}

    DynLib* DynLibManager::load(const String& name)
    {
		String filename = name;
#if BS_PLATFORM == BS_PLATFORM_LINUX
		if (name.substr(name.length() - 3, 3) != ".so")
			name += ".so";
#elif BS_PLATFORM == BS_PLATFORM_OSX
		if (name.substr(name.length() - 6, 6) != ".dylib")
			name += ".dylib";
#elif BS_PLATFORM == BS_PLATFORM_WIN32
		// Although LoadLibraryEx will add .dll itself when you only specify the library name,
		// if you include a relative path then it does not. So, add it to be sure.
		if (filename.substr(filename.length() - 4, 4) != ".dll")
			filename += ".dll";
#endif

		auto iterFind = mLoadedLibraries.find(filename);
		if (iterFind != mLoadedLibraries.end())
		{
			return iterFind->second;
		}
		else
		{
	        DynLib* newLib = new (bs_alloc<DynLib>()) DynLib(filename);
        	mLoadedLibraries[filename] = newLib;

	        return newLib;
		}
    }

	void DynLibManager::unload(DynLib* lib)
	{
		auto iterFind = mLoadedLibraries.find(lib->getName());
		if (iterFind != mLoadedLibraries.end())
		{
			mLoadedLibraries.erase(iterFind);
		}

		lib->unload();
		bs_delete(lib);
	}

	DynLibManager::~DynLibManager()
    {
        // Unload & delete resources in turn
        for(auto& entry : mLoadedLibraries)
        {
            entry.second->unload();
			bs_delete(entry.second);
        }

        // Empty the list
        mLoadedLibraries.clear();
    }

	DynLibManager& gDynLibManager()
	{
		return DynLibManager::instance();
	}
}
