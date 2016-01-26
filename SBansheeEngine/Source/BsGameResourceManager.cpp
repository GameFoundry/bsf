//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsGameResourceManager.h"
#include "BsResources.h"

namespace BansheeEngine
{
	HResource StandaloneResourceLoader::load(const Path& path, bool keepLoaded) const
	{
		return gResources().load(path, true, keepLoaded);
	}

	GameResourceManager::GameResourceManager()
		:mLoader(bs_shared_ptr_new<StandaloneResourceLoader>())
	{
		
	}

	HResource GameResourceManager::load(const Path& path, bool keepLoaded) const
	{
		return mLoader->load(path, keepLoaded);
	}

	void GameResourceManager::setLoader(const SPtr<IGameResourceLoader>& loader)
	{
		mLoader = loader;

		if (mLoader == nullptr)
			mLoader = bs_shared_ptr_new<StandaloneResourceLoader>();
	}
}