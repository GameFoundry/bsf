//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "RenderAPI/BsBlendState.h"
#include "Managers/BsRenderStateManager.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT BlendStateRTTI : public RTTIType<BlendState, IReflectable, BlendStateRTTI>
	{
	private:
		BLEND_STATE_DESC& getData(BlendState* obj) { return obj->mProperties.mData; }
		void setData(BlendState* obj, BLEND_STATE_DESC& val) { obj->mProperties.mData = val; } 

	public:
		BlendStateRTTI()
		{
			addPlainField("mData", 0, &BlendStateRTTI::getData, &BlendStateRTTI::setData);
		}

		void onDeserializationEnded(IReflectable* obj, const UnorderedMap<String, UINT64>& params) override
		{
			BlendState* blendState = static_cast<BlendState*>(obj);
			blendState->initialize();
		}

		const String& getRTTIName() override
		{
			static String name = "BlendState";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_BlendState;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return RenderStateManager::instance()._createBlendStatePtr(BLEND_STATE_DESC());
		}
	};

	/** @} */
	/** @endcond */
}