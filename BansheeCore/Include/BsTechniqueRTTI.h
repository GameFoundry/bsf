#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsTechnique.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT TechniqueRTTI : public RTTIType<Technique, IReflectable, TechniqueRTTI>
	{
	private:
		StringID& getRenderSystem(Technique* obj) { return obj->mRenderAPI; }
		void setRenderSystem(Technique* obj, StringID& val) { obj->mRenderAPI = val; }

		StringID& getRenderer(Technique* obj) { return obj->mRenderer; }
		void setRenderer(Technique* obj, StringID& val) { obj->mRenderer = val; }

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

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			Technique* technique = static_cast<Technique*>(obj);
			technique->initialize();
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
			return Technique::createEmpty();
		}
	};
}