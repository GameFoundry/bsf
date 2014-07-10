//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsTechnique.h"
#include "BsException.h"
#include "BsRenderSystem.h"
#include "BsRendererManager.h"
#include "BsPass.h"
#include "BsRenderer.h"
#include "BsTechniqueRTTI.h"

namespace BansheeEngine
{
	Technique::Technique(const String& renderSystem, const String& renderer)
		:mRenderSystem(renderSystem), mRenderer(renderer)
	{

	}

	PassPtr Technique::addPass()
	{
		PassPtr newPass = bs_shared_ptr<Pass, PoolAlloc>();

		mPasses.push_back(newPass);
		return newPass;
	}

	void Technique::removePass(UINT32 idx)
	{
		if(idx < 0 || idx >= mPasses.size())
			BS_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx));

		int count = 0;
		auto iter = mPasses.begin();
		while(count != idx)
		{
			++count;
			++iter;
		}

		mPasses.erase(iter);
	}

	PassPtr Technique::getPass(UINT32 idx) const
	{
		if(idx < 0 || idx >= mPasses.size())
			BS_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx));

		return mPasses[idx];
	}

	bool Technique::isSupported() const
	{
		if(RenderSystem::instancePtr()->getName() == mRenderSystem &&
			(RendererManager::instance().getActive()->getName() == mRenderer ||
			 RendererManager::getCoreRendererName() == mRenderer))
		{
			return true;
		}

		return false;
	}

	RTTITypeBase* Technique::getRTTIStatic()
	{
		return TechniqueRTTI::instance();
	}

	RTTITypeBase* Technique::getRTTI() const
	{
		return Technique::getRTTIStatic();
	}
}