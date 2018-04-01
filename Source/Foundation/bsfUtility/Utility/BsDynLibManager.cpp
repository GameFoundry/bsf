//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Utility/BsDynLibManager.h"
#include "Utility/BsDynLib.h"

namespace bs
{
	DynLib* DynLibManager::load(String filename)
	{
		// Add the extension (.dll, .so, ...) if necessary.
		// TODO: The string comparison here could be slightly more efficent
		// Using a templatized string_concat function for the lower_bound call
		// and/or a custom comparitor that does comparison by parts.
		const String::size_type length = filename.length();
		const String extension = String(".") + DynLib::EXTENSION;
		const String::size_type extLength = extension.length();
		if (length <= extLength || filename.substr(length - extLength) != extension)
			filename.append(extension);

		if(DynLib::PREFIX != nullptr)
			filename.insert(0, DynLib::PREFIX);

		const auto& iterFind = mLoadedLibraries.lower_bound(filename);
		if (iterFind != mLoadedLibraries.end() && iterFind->first == filename)
		{
			return iterFind->second;
		}
		else
		{
			DynLib* newLib = new (bs_alloc<DynLib>()) DynLib(filename);
			mLoadedLibraries.emplace_hint(iterFind, std::move(filename), newLib);
			return newLib;
		}
	}

	void DynLibManager::unload(DynLib* lib)
	{
		const auto& iterFind = mLoadedLibraries.find(lib->getName());
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
