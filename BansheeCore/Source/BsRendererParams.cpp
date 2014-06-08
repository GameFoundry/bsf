#include "BsRendererParams.h"

namespace BansheeEngine
{
	RendererMaterialParam::RendererMaterialParam(UINT32 semantic, GpuProgramType program, bool isObject, UINT32 passIdx, UINT32 blockSemantic)
		:semantic(semantic), program(program), isObject(isObject), passIdx(passIdx), blockSemantic(blockSemantic)
	{

	}

	RendererMaterialDataParam::RendererMaterialDataParam(UINT32 semantic, GpuProgramType program, GpuParamDataType type, UINT32 elementSize,
		UINT32 arraySize, UINT32 arrayElementStride, UINT32 cpuOffset, UINT32 gpuOffset, UINT32 passIdx, UINT32 blockSemantic)
		: RendererMaterialParam(semantic, program, false, passIdx, blockSemantic), type(type), elementSize(elementSize), arraySize(arraySize),
		arrayElementStride(arrayElementStride), cpuOffset(cpuOffset), gpuOffset(gpuOffset)
	{

	}

	RendererMaterialObjectParam::RendererMaterialObjectParam(UINT32 semantic, GpuProgramType program, GpuParamObjectType type,
		UINT32 slot, UINT32 passIdx, UINT32 blockSemantic)
		: RendererMaterialParam(semantic, program, true, passIdx, blockSemantic), type(type), slot(slot)
	{

	}

	RendererMaterialParams::RendererMaterialParams(const String& renderSystem, UINT32 renderableType)
		:mRenderSystem(renderSystem), mRenderableType(renderableType)
	{ }

	RendererMaterialParams::~RendererMaterialParams()
	{
		for (auto& param : mParams)
		{
			bs_delete(param);
		}
	}

	bool RendererMaterialParams::operator== (const RendererMaterialParams& params) const
	{
		return mRenderSystem == params.mRenderSystem && mRenderableType == params.mRenderableType;
	}

	void RendererMaterialParams::addDataParam(UINT32 semantic, GpuProgramType program, GpuParamDataType type, UINT32 elementSize, 
		UINT32 cpuOffset, UINT32 gpuOffset, UINT32 passIdx, UINT32 blockSemantic)
	{
		mParams.push_back(bs_new<RendererMaterialDataParam>(semantic, program, type, elementSize, 1, elementSize, cpuOffset, gpuOffset, passIdx, blockSemantic));
	}

	void RendererMaterialParams::addArrayDataParam(UINT32 semantic, GpuProgramType program, GpuParamDataType type, UINT32 elementSize,
		UINT32 arraySize, UINT32 arrayElementStride, UINT32 cpuOffset, UINT32 gpuOffset, UINT32 passIdx, UINT32 blockSemantic)
	{
		mParams.push_back(bs_new<RendererMaterialDataParam>(semantic, program, type, elementSize, arraySize, arrayElementStride, 
			cpuOffset, gpuOffset, passIdx, blockSemantic));
	}

	void RendererMaterialParams::addObjectParam(UINT32 semantic, GpuProgramType program, GpuParamObjectType type,
		UINT32 slot, UINT32 passIdx, UINT32 blockSemantic)
	{
		mParams.push_back(bs_new<RendererMaterialObjectParam>(semantic, program, type, slot, passIdx, blockSemantic));
	}
}