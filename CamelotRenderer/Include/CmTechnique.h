#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class CM_EXPORT Technique
	{
	public:
		Technique(const String& renderSystem, const String& renderManager);

		void addPass(PassPtr pass);
		void removePass(UINT32 idx);

		UINT32 getNumPassess() const { return mPasses.size(); }

	private:
		String mRenderSystem;
		String mRenderManager;

		vector<PassPtr>::type mPasses;
	};
}