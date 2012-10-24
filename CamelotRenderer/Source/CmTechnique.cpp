#include "CmTechnique.h"
#include "CmException.h"
#include "CmRenderSystemManager.h"
#include "CmRenderSystem.h"
#include "CmRendererManager.h"
#include "CmRenderer.h"

namespace CamelotEngine
{
	Technique::Technique(const String& renderSystem, const String& renderer)
		:mRenderSystem(renderSystem), mRenderer(renderer)
	{

	}

	void Technique::addPass(PassPtr pass)
	{
		mPasses.push_back(pass);
	}

	void Technique::removePass(UINT32 idx)
	{
		if(idx < 0 || idx >= mPasses.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx));

		int count = 0;
		auto iter = mPasses.begin();
		while(count != idx)
		{
			++count;
			++iter;
		}

		mPasses.erase(iter);
	}

	PassPtr Technique::getPass(UINT32 idx)
	{
		if(idx < 0 || idx >= mPasses.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx));

		return mPasses[idx];
	}

	bool Technique::isSupported() const
	{
		if(RenderSystemManager::getActive()->getName() == mRenderSystem &&
			RendererManager::getActive()->getName() == mRenderer)
		{
			return true;
		}

		return false;
	}
}