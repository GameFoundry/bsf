#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuProgram.h"
#include "BsUtil.h"
#include "BsGpuParamDesc.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	enum RendererBlockSemantic
	{
		RBS_Occassional,
		RBS_PerFrame,
		RBS_PerObject
	};

	enum RendererParamSemantic
	{
		RPS_WorldViewProjTfrm,
		RPS_ViewProjTfrm,
		RPS_WorldTfrm
	};

	enum RendererPass
	{
		RP_AnyPass = 0xFFFFFFFF
	};

	enum RendererBlock
	{
		RB_NoBlock = 0xFFFFFFFF
	};

	struct BS_CORE_EXPORT RendererMaterialParam
	{
		RendererMaterialParam(UINT32 semantic, GpuProgramType program, bool isObject, UINT32 passIdx = RP_AnyPass, UINT32 blockSemantic = RB_NoBlock);
		virtual ~RendererMaterialParam() {}

		UINT32 semantic;
		GpuProgramType program;
		UINT32 passIdx; /**< 0xFFFFFFFF any pass. */
		UINT32 blockSemantic; /**< 0xFFFFFFFF no block. */
		bool isObject;
	};

	struct BS_CORE_EXPORT RendererMaterialDataParam : public RendererMaterialParam
	{
		RendererMaterialDataParam(UINT32 semantic, GpuProgramType program, GpuParamDataType type, UINT32 elementSize,
			UINT32 arraySize, UINT32 arrayElementStride, UINT32 cpuOffset, UINT32 gpuOffset,
			UINT32 passIdx = RP_AnyPass, UINT32 blockSemantic = RB_NoBlock);

		GpuParamDataType type;
		UINT32 elementSize;
		UINT32 arraySize;
		UINT32 arrayElementStride;
		UINT32 cpuOffset; 
		UINT32 gpuOffset;
	};

	struct BS_CORE_EXPORT RendererMaterialObjectParam : public RendererMaterialParam
	{
		RendererMaterialObjectParam(UINT32 semantic, GpuProgramType program, GpuParamObjectType type,
			UINT32 slot, UINT32 passIdx = RP_AnyPass, UINT32 blockSemantic = RB_NoBlock);

		GpuParamObjectType type;
		UINT32 slot;
	};

	class BS_CORE_EXPORT RendererMaterialParams
	{
	public:
		RendererMaterialParams(const String& renderSystem, UINT32 renderableType);
		~RendererMaterialParams();

		bool operator== (const RendererMaterialParams& params) const;

		void addDataParam(UINT32 semantic, GpuProgramType program, GpuParamDataType type, UINT32 elementSize,
			UINT32 cpuOffset, UINT32 gpuOffset,	UINT32 passIdx = RP_AnyPass, UINT32 blockSemantic = RB_NoBlock);

		void addArrayDataParam(UINT32 semantic, GpuProgramType program, GpuParamDataType type, UINT32 elementSize,
			UINT32 arraySize, UINT32 arrayElementStride, UINT32 cpuOffset, UINT32 gpuOffset, UINT32 passIdx = RP_AnyPass, 
			UINT32 blockSemantic = RB_NoBlock);

		void addObjectParam(UINT32 semantic, GpuProgramType program, GpuParamObjectType type,
			UINT32 slot, UINT32 passIdx = RP_AnyPass, UINT32 blockSemantic = RB_NoBlock);

	private:
		friend struct std::hash<RendererMaterialParams>;

		Vector<RendererMaterialParam*> mParams;
		String mRenderSystem;
		UINT32 mRenderableType;
	};
}

template<>
struct std::hash<BansheeEngine::RendererMaterialParams>
{
	size_t operator()(const BansheeEngine::RendererMaterialParams& params) const
	{
		size_t hash = 0;
		BansheeEngine::hash_combine(hash, params.mRenderSystem);
		BansheeEngine::hash_combine(hash, params.mRenderableType);

		return hash;
	}
};