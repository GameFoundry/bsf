#include "CmDynLibManager.h"
#include "CmDynLib.h"

namespace BansheeEngine
{
	DynLibManager::DynLibManager()
	{
	}

    DynLib* DynLibManager::load( const String& filename)
    {
		auto iterFind = mLoadedLibraries.find(filename);
		if (iterFind != mLoadedLibraries.end())
		{
			return iterFind->second;
		}
		else
		{
	        DynLib* newLib = new (bs_alloc<DynLib>()) DynLib(filename);

			newLib->load();       
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
