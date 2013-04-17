#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmTechnique.h"

namespace CamelotFramework
{
	class CM_EXPORT TechniqueRTTI : public RTTIType<Technique, IReflectable, TechniqueRTTI>
	{
	private:
		String& getRenderSystem(Technique* obj) { return obj->mRenderSystem; }
		void setRenderSystem(Technique* obj, String& val) { obj->mRenderSystem = val; } 

		String& getRenderer(Technique* obj) { return obj->mRenderer; }
		void setRenderer(Technique* obj, String& val) { obj->mRenderer = val; } 

		PassPtr getPass(Technique* obj, UINT32 idx)
		{
			return obj->mPasses[idx];
		}

		void setPass(Technique* obj, UINT32 idx, PassPtr val)
		{
			obj->mPasses[idx] = val;
		}

		UINT32 getPassArraySize(Technique* obj)
		{
			return (UINT32)obj->mPasses.size();
		}

		void setPassArraySize(Technique* obj, UINT32 size)
		{
			obj->mPasses.resize(size);
		}

	public:
		TechniqueRTTI()
		{
			addPlainField("mRenderSystem", 0, &TechniqueRTTI::getRenderSystem, &TechniqueRTTI::setRenderSystem);
			addPlainField("mRenderer", 1, &TechniqueRTTI::getRenderer, &TechniqueRTTI::setRenderer);

			addReflectablePtrArrayField("mPasses", 2, &TechniqueRTTI::getPass, &TechniqueRTTI::getPassArraySize, &TechniqueRTTI::setPass, &TechniqueRTTI::setPassArraySize);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Technique";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Technique;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<Technique>(CM_NEW(Technique, PoolAlloc) Technique(),
				&MemAllocDeleter<Technique, PoolAlloc>::deleter);
		}
	};
}