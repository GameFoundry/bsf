#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParams.h"
#include "BsRenderAPI.h"
#include "BsGpuParamBlockBuffer.h"

namespace BansheeEngine
{
	/**
	 * Macros used for manually constructing parameter block buffers used by GPU programs.
	 */

#define BS_PARAM_BLOCK_BEGIN(Name)																							\
	struct Name																												\
	{																														\
		Name()																												\
		{																													\
			Vector<GpuParamDataDesc> params = getEntries();																	\
			RenderAPICore& rapi = RenderAPICore::instance();																\
																															\
			mBlockDesc = rapi.generateParamBlockDesc(#Name, params);														\
																															\
			SPtr<GpuParamDesc> paramsDesc = bs_shared_ptr_new<GpuParamDesc>();												\
			paramsDesc->paramBlocks[#Name] = mBlockDesc;																	\
			for (auto& param : params)																						\
				paramsDesc->params[param.name] = param;																		\
																															\
			mParams = GpuParamsCore::create(paramsDesc, rapi.getGpuProgramHasColumnMajorMatrices());						\
																															\
			mBuffer = GpuParamBlockBufferCore::create(mBlockDesc.blockSize * sizeof(UINT32));								\
			mParams->setParamBlockBuffer(#Name, mBuffer);																	\
			initEntries();																									\
		}																													\
																															\
		const SPtr<GpuParamBlockBufferCore>& getBuffer() const { return mBuffer; }											\
		const GpuParamBlockDesc& getDesc() const { return mBlockDesc; }														\
																															\
	private:																												\
		struct META_FirstEntry {};																							\
		static void META_GetPrevEntries(Vector<GpuParamDataDesc>& params, META_FirstEntry id) { }							\
		void META_InitPrevEntry(const SPtr<GpuParamsCore>& params, META_FirstEntry id) { }									\
																															\
		typedef META_FirstEntry 

#define BS_PARAM_BLOCK_ENTRY_ARRAY(Type, Name, NumElements)																	\
		META_Entry_##Name;																									\
																															\
		struct META_NextEntry_##Name {};																					\
		static void META_GetPrevEntries(Vector<GpuParamDataDesc>& params, META_NextEntry_##Name id)							\
		{																													\
			META_GetPrevEntries(params, META_Entry_##Name##());																\
																															\
			params.push_back(GpuParamDataDesc());																			\
			GpuParamDataDesc& newEntry = params.back();																		\
			newEntry.name = #Name;																							\
			newEntry.type = (GpuParamDataType)TGpuDataParamInfo<Type>::TypeId;												\
			newEntry.arraySize = NumElements;																				\
		}																													\
																															\
		void META_InitPrevEntry(const SPtr<GpuParamsCore>& params, META_NextEntry_##Name id)								\
		{																													\
			META_InitPrevEntry(params, META_Entry_##Name##());																\
			params->getParam(#Name, Name);																					\
		}																													\
																															\
	public:																													\
		TGpuDataParam<Type, true> Name;																						\
																															\
	private:																												\
		typedef META_NextEntry_##Name

#define BS_PARAM_BLOCK_ENTRY(Type, Name) BS_PARAM_BLOCK_ENTRY_ARRAY(Type, Name, 1)

#define BS_PARAM_BLOCK_END																									\
		META_LastEntry;																										\
																															\
		static Vector<GpuParamDataDesc> getEntries()																		\
		{																													\
			Vector<GpuParamDataDesc> entries;																				\
			META_GetPrevEntries(entries, META_LastEntry());																	\
			return entries;																									\
		}																													\
																															\
		void initEntries()																									\
		{																													\
			META_InitPrevEntry(mParams, META_LastEntry());																	\
		}																													\
																															\
		SPtr<GpuParamsCore> mParams;																						\
		SPtr<GpuParamBlockBufferCore> mBuffer;																				\
		GpuParamBlockDesc mBlockDesc;																						\
	};
}