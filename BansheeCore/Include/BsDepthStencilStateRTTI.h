#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsDepthStencilState.h"
#include "BsRenderStateManager.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT DepthStencilStateRTTI : public RTTIType<DepthStencilState, IReflectable, DepthStencilStateRTTI>
	{
	private:
		DEPTH_STENCIL_STATE_DESC& getData(DepthStencilState* obj) { return obj->mProperties.mData; }
		void setData(DepthStencilState* obj, DEPTH_STENCIL_STATE_DESC& val) { obj->mProperties.mData = val; } 

	public:
		DepthStencilStateRTTI()
		{
			addPlainField("mData", 0, &DepthStencilStateRTTI::getData, &DepthStencilStateRTTI::setData);
		}

		void onDeserializationEnded(IReflectable* obj) override
		{
			DepthStencilState* depthStencilState = static_cast<DepthStencilState*>(obj);
			depthStencilState->initialize();
		}

		const String& getRTTIName() override
		{
			static String name = "DepthStencilState";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_DepthStencilState;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return RenderStateManager::instance()._createDepthStencilStatePtr(DEPTH_STENCIL_STATE_DESC());
		}
	};

	/** @} */
	/** @endcond */
}