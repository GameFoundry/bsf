#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsDepthStencilState.h"
#include "BsRenderStateManager.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT DepthStencilStateRTTI : public RTTIType<DepthStencilState, IReflectable, DepthStencilStateRTTI>
	{
	private:
		DEPTH_STENCIL_STATE_DESC& getData(DepthStencilState* obj) { return obj->mData; }
		void setData(DepthStencilState* obj, DEPTH_STENCIL_STATE_DESC& val) 
		{ 
			obj->mRTTIData = val;
		} 

	public:
		DepthStencilStateRTTI()
		{
			addPlainField("mData", 0, &DepthStencilStateRTTI::getData, &DepthStencilStateRTTI::setData);
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			DepthStencilState* depthStencilState = static_cast<DepthStencilState*>(obj);
			if(!depthStencilState->mRTTIData.empty())
			{
				DEPTH_STENCIL_STATE_DESC desc = any_cast<DEPTH_STENCIL_STATE_DESC>(depthStencilState->mRTTIData);

				depthStencilState->initialize(desc);
			}
		}

		virtual const String& getRTTIName()
		{
			static String name = "DepthStencilState";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_DepthStencilState;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return RenderStateManager::instance().createEmptyDepthStencilState();
		}
	};
}