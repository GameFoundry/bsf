#include "BsRenderTargetManager.h"
#include "BsRenderTarget.h"

namespace BansheeEngine
{
	RenderTargetManager::~RenderTargetManager()
	{
		assert(mRenderTargets.size() == 0 && "All render targets must be released before render target manager can be shut down.");
	}

	void RenderTargetManager::registerRenderTarget(RenderTarget* rt)
	{
		BS_LOCK_MUTEX(mMutex);

		RenderTargetData rtData;
		rtData.properties = rt->createProperties();
		rtData.dirty = true;

		mRenderTargets.insert(std::make_pair(rt, rtData));
	}

	void RenderTargetManager::unregisterRenderTarget(RenderTarget* rt)
	{
		BS_LOCK_MUTEX(mMutex);

		auto findIter = mRenderTargets.find(rt);
		if (findIter == mRenderTargets.end())
			return;

		RenderTargetData& rtData = findIter->second;
		bs_delete(rtData.properties);

		mRenderTargets.erase(findIter);
	}

	void RenderTargetManager::updateCore()
	{
		BS_LOCK_MUTEX(mMutex);

		for (auto& rtPair : mRenderTargets)
		{
			RenderTarget* rt = rtPair.first;
			RenderTargetData& rtData = rtPair.second;

			if (rt->getCore()->_isCoreDirty())
			{
				*rtData.properties = rt->getCore()->getProperties();
				rtData.dirty = true;

				rt->getCore()->_markCoreClean();
			}
		}
	}

	void RenderTargetManager::update()
	{
		BS_LOCK_MUTEX(mMutex);

		for (auto& rtPair : mRenderTargets)
		{
			RenderTarget* rt = rtPair.first;
			RenderTargetData& rtData = rtPair.second;

			if (rtData.dirty)
			{
				*rt->mProperties = *rtData.properties;

				rtData.dirty = false;
			}
		}
	}
}