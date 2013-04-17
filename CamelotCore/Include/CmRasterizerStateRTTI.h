#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmRasterizerState.h"
#include "CmRenderStateManager.h"

namespace CamelotFramework
{
	CM_ALLOW_MEMCPY_SERIALIZATION(RASTERIZER_STATE_DESC);

	class CM_EXPORT RasterizerStateRTTI : public RTTIType<RasterizerState, IReflectable, RasterizerStateRTTI>
	{
	private:
		RASTERIZER_STATE_DESC& getData(RasterizerState* obj) { return obj->mData; }
		void setData(RasterizerState* obj, RASTERIZER_STATE_DESC& val) 
		{ 
			obj->mRTTIData = val;
		} 

	public:
		RasterizerStateRTTI()
		{
			addPlainField("mData", 0, &RasterizerStateRTTI::getData, &RasterizerStateRTTI::setData);
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			RasterizerState* rasterizerState = static_cast<RasterizerState*>(obj);
			if(!rasterizerState->mRTTIData.empty())
			{
				RASTERIZER_STATE_DESC desc = boost::any_cast<RASTERIZER_STATE_DESC>(rasterizerState->mRTTIData);

				rasterizerState->initialize(desc);
			}
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