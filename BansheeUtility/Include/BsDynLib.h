#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsString.h"

#if BS_PLATFORM == BS_PLATFORM_WIN32
#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD( a ) LoadLibraryEx( a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define DYNLIB_UNLOAD( a ) !FreeLibrary( a )

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif BS_PLATFORM == BS_PLATFORM_LINUX
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )

#elif BS_PLATFORM == BS_PLATFORM_APPLE
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) mac_loadDylib( a )
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )

#endif

namespace BansheeEngine 
{
    /** 
	 * @brief	Class that holds data about a dynamic library.
	 */
	class BS_UTILITY_EXPORT DynLib
    {

    public:
        ~DynLib();

		/** 
		 * @brief	Loads the library. Does nothing if library is already loaded.
		 */
        void load();

		/**
		 * @brief	Unloads the library. Does nothing if library is not loaded.
		 */
        void unload();

		/** 
		 * @brief	Get the name of the library.
		 */
		const String& getName() const { return mName; }

        /**
		 * @brief	Returns the address of the given symbol from the loaded library.
		 *
		 * @param strName	The name of the symbol to search for.
		 *
         * @returns		If the function succeeds, the returned value is a handle to
		 *				the symbol. Otherwise null.
         */
        void* getSymbol(const String& strName) const;

	protected:
		friend class DynLibManager;

		DynLib(const String& name);

		/** 
		 * @brief	Gets the last loading error.
		 */
        String dynlibError();

    protected:
		String mName;
        DYNLIB_HANDLE m_hInst; // Handle to the loaded library.
    };
}