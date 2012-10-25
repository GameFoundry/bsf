#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class CM_EXPORT Technique
	{
	public:
		Technique(const String& renderSystem, const String& renderer);

		PassPtr addPass();
		void removePass(UINT32 idx);
		PassPtr getPass(UINT32 idx) const;

		UINT32 getNumPasses() const { return mPasses.size(); }

		bool isSupported() const;

	private:
		String mRenderSystem;
		String mRenderer;

		vector<PassPtr>::type mPasses;
	};
}