#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmModule.h"

namespace BansheeEngine 
{
    /**
	 * @brief	This manager keeps a track of all the open dynamic-loading
	 *			libraries, opens them and returns references to already-open
	 *			libraries.
	 *			
	 * @note	Not thread safe.
     */
    class CM_UTILITY_EXPORT DynLibManager : public Module<DynLibManager>
    {
    public:
        DynLibManager();
        virtual ~DynLibManager();

        /**
         * @brief	Loads the given file as a dynamic library.
         *
         * @param	filename	The name of the library. The extension can be omitted
         */
        DynLib* load(const String& filename);

		/**
		 * @brief	Unloads the given library.
		 */
		void unload(DynLib* lib);

	protected:
		Map<String, DynLib*> mLoadedLibraries;
    };

	CM_UTILITY_EXPORT DynLibManager& gDynLibManager();
}