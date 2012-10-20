#include "CmTechnique.h"
#include "CmException.h"

namespace CamelotEngine
{
	Technique::Technique(const String& renderSystem, const String& renderManager)
		:mRenderSystem(renderSystem), mRenderManager(renderManager)
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
}