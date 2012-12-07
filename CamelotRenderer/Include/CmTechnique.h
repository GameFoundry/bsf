#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	class CM_EXPORT Technique : public IReflectable
	{
	public:
		Technique(const String& renderSystem, const String& renderer);

		PassPtr addPass();
		void removePass(UINT32 idx);
		PassPtr getPass(UINT32 idx) const;

		UINT32 getNumPasses() const { return (UINT32)mPasses.size(); }

		bool isSupported() const;

	private:
		String mRenderSystem;
		String mRenderer;

		vector<PassPtr>::type mPasses;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		/**
		 * @brief	Serialization only constructor.
		 */
		Technique() {}

	public:
		friend class TechniqueRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}