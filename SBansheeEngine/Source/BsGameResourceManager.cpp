#include "BsGameResourceManager.h"
#include "BsResources.h"

namespace BansheeEngine
{
	HResource StandaloneResourceLoader::load(const Path& path) const
	{
		return gResources().load(path);
	}

	GameResourceManager::GameResourceManager()
		:mLoader(bs_shared_ptr_new<StandaloneResourceLoader>())
	{
		
	}

	HResource GameResourceManager::load(const Path& path) const
	{
		return mLoader->load(path);
	}

	void GameResourceManager::setLoader(const SPtr<IGameResourceLoader>& loader)
	{
		mLoader = loader;

		if (mLoader == nullptr)
			mLoader = bs_shared_ptr_new<StandaloneResourceLoader>();
	}
}