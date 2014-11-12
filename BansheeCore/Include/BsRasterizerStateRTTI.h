#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsRasterizerState.h"
#include "BsRenderStateManager.h"

namespace BansheeEngine
{
	BS_ALLOW_MEMCPY_SERIALIZATION(RASTERIZER_STATE_DESC);

	class BS_CORE_EXPORT RasterizerStateRTTI : public RTTIType<RasterizerState, IReflectable, RasterizerStateRTTI>
	{
	private:
		RASTERIZER_STATE_DESC& getData(RasterizerState* obj) { return obj->mProperties.mData; }
		void setData(RasterizerState* obj, RASTERIZER_STATE_DESC& val) { obj->mProperties.mData = val; } 

	public:
		RasterizerStateRTTI()
		{
			addPlainField("mData", 0, &RasterizerStateRTTI::getData, &RasterizerStateRTTI::setData);
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			RasterizerState* rasterizerState = static_cast<RasterizerState*>(obj);
			rasterizerState->initialize();
		}

		virtual const String& getRTTIName()
		{
			static String name = "RasterizerState";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_RasterizerState;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return RenderStateManager::instance().createEmptyRasterizerState();
		}
	};
}