//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsTechnique.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT TechniqueRTTI : public RTTIType<Technique, IReflectable, TechniqueRTTI>
	{
	private:
		StringID& getRenderAPI(Technique* obj) { return obj->mRenderAPI; }
		void setRenderAPI(Technique* obj, StringID& val) { obj->mRenderAPI = val; }

		StringID& getRenderer(Technique* obj) { return obj->mRenderer; }
		void setRenderer(Technique* obj, StringID& val) { obj->mRenderer = val; }

		SPtr<Pass> getPass(Technique* obj, UINT32 idx) { return obj->mPasses[idx]; }
		void setPass(Technique* obj, UINT32 idx, SPtr<Pass> val) { obj->mPasses[idx] = val; }

		UINT32 getPassArraySize(Technique* obj) { return (UINT32)obj->mPasses.size(); }
		void setPassArraySize(Technique* obj, UINT32 size) { obj->mPasses.resize(size); }

	public:
		TechniqueRTTI()
		{
			addPlainField("mRenderAPI", 0, &TechniqueRTTI::getRenderAPI, &TechniqueRTTI::setRenderAPI);
			addPlainField("mRenderer", 1, &TechniqueRTTI::getRenderer, &TechniqueRTTI::setRenderer);

			addReflectablePtrArrayField("mPasses", 2, &TechniqueRTTI::getPass, &TechniqueRTTI::getPassArraySize, &TechniqueRTTI::setPass, &TechniqueRTTI::setPassArraySize);
		}

		void onDeserializationEnded(IReflectable* obj, const UnorderedMap<String, UINT64>& params) override
		{
			Technique* technique = static_cast<Technique*>(obj);
			technique->initialize();
		}

		const String& getRTTIName() override
		{
			static String name = "Technique";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_Technique;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return Technique::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}