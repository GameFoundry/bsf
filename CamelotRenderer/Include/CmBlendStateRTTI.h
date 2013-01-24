#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmBlendState.h"
#include "CmRenderStateManager.h"

namespace CamelotEngine
{
	template<> struct SerializableSimpleType<BLEND_STATE_DESC>
	{	
		enum { id = TID_BLEND_STATE_DESC }; enum { hasDynamicSize = 1 };

		static void toMemory(BLEND_STATE_DESC& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			memcpy(memory, &size, sizeof(UINT32));
			memory += sizeof(UINT32);
			size -= sizeof(UINT32);
			memcpy(memory, &data, size); 
		}

		static void fromMemory(BLEND_STATE_DESC& data, char* memory)
		{ 
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);

			size -= sizeof(UINT32);
			memcpy((void*)&data, memory, size); 
		}

		static UINT32 getDynamicSize(BLEND_STATE_DESC& data)	
		{ 
			UINT64 dataSize = sizeof(data) + sizeof(UINT32);

#if CM_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				CM_EXCEPT(InternalErrorException, "Data overflow! Size doesn't fit into 32 bits.");
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

	class CM_EXPORT BlendStateRTTI : public RTTIType<BlendState, IReflectable, BlendStateRTTI>
	{
	private:
		BLEND_STATE_DESC& getData(BlendState* obj) { return obj->mData; }
		void setData(BlendState* obj, BLEND_STATE_DESC& val) 
		{ 
			obj->mRTTIData = val;
		} 

	public:
		BlendStateRTTI()
		{
			addPlainField("mData", 0, &BlendStateRTTI::getData, &BlendStateRTTI::setData);
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			BlendState* blendState = static_cast<BlendState*>(obj);
			if(!blendState->mRTTIData.empty())
			{
				BLEND_STATE_DESC desc = boost::any_cast<BLEND_STATE_DESC>(blendState->mRTTIData);

				blendState->initialize(desc);
			}

		}

		virtual const String& getRTTIName()
		{
			static String name = "BlendState";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_BlendState;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return RenderStateManager::instance().createEmptyBlendState();
		}
	};
}