#include "CmDynLibManager.h"
#include "CmDynLib.h"

namespace CamelotFramework
{
	DynLibManager::DynLibManager()
	{
	}

    DynLib* DynLibManager::load( const String& filename)
    {
		DynLibList::iterator i = mLibList.find(filename);
		if (i != mLibList.end())
		{
			return i->second;
		}
		else
		{
	        DynLib* pLib = cm_new<DynLib>(filename);
			pLib->load();        
        	mLibList[filename] = pLib;
	        return pLib;
		}
    }

	void DynLibManager::unload(DynLib* lib)
	{
		DynLibList::iterator i = mLibList.find(lib->getName());
		if (i != mLibList.end())
		{
			mLibList.erase(i);
		}
		lib->unload();
		cm_delete(lib);
	}

	DynLibManager::~DynLibManager()
    {
        // Unload & delete resources in turn
        for( DynLibList::iterator it = mLibList.begin(); it != mLibList.end(); ++it )
        {
            it->second->unload();
			cm_delete(it->second);
        }

        // Empty the list
        mLibList.clear();
    }

	DynLibManager& gDynLibManager()
	{
		return DynLibManager::instance();
	}
}
