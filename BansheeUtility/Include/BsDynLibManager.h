//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsModule.h"

namespace BansheeEngine 
{
    /**
	 * @brief	This manager keeps a track of all the open dynamic-loading
	 *			libraries, opens them and returns references to already-open
	 *			libraries.
	 *			
	 * @note	Not thread safe.
     */
    class BS_UTILITY_EXPORT DynLibManager : public Module<DynLibManager>
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

	BS_UTILITY_EXPORT DynLibManager& gDynLibManager();
}