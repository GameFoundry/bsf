#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsSamplerState.h"
#include "BsRenderStateManager.h"

namespace BansheeEngine
{
	BS_ALLOW_MEMCPY_SERIALIZATION(SAMPLER_STATE_DESC);

	class BS_CORE_EXPORT SamplerStateRTTI : public RTTIType<SamplerState, IReflectable, SamplerStateRTTI>
	{
	private:
		SAMPLER_STATE_DESC& getData(SamplerState* obj) { return obj->mProperties.mData; }
		void setData(SamplerState* obj, SAMPLER_STATE_DESC& val) { obj->mProperties.mData = val; } 

	public:
		SamplerStateRTTI()
		{
			addPlainField("mData", 0, &SamplerStateRTTI::getData, &SamplerStateRTTI::setData);
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			SamplerState* samplerState = static_cast<SamplerState*>(obj);
			samplerState->initialize();
		}

		virtual const String& getRTTIName()
		{
			static String name = "SamplerState";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SamplerState;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return RenderStateManager::instance().createEmptySamplerState();
		}
	};
}