//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsLightGrid.h"
#include "BsGpuBuffer.h"
#include "BsGpuParamsSet.h"
#include "BsRendererUtility.h"
#include "BsRendererCamera.h"
#include "BsRenderTargets.h"
#include "BsLightRendering.h"

namespace bs { namespace ct
{
	static const UINT32 CELL_XY_SIZE = 64;
	static const UINT32 NUM_Z_SUBDIVIDES = 32;
	static const UINT32 MAX_LIGHTS_PER_CELL = 32;
	static const UINT32 THREADGROUP_SIZE = 4;

	LightGridParamDef gLightGridParamDefDef;

	LightGridLLCreationMat::LightGridLLCreationMat()
		:mBufferNumCells(0)
	{
		SPtr<GpuParams> params = mParamsSet->getGpuParams();

		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gLights", mLightBufferParam);
		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gLinkedListCounter", mLinkedListCounterParam);
		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gLinkedListHeads", mLinkedListHeadsParam);
		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gLinkedList", mLinkedListParam);

		GPU_BUFFER_DESC desc;
		desc.elementCount = 1;
		desc.format = BF_32X1U;
		desc.randomGpuWrite = true;
		desc.type = GBT_STANDARD;
		desc.elementSize = 0;

		mLinkedListCounter = GpuBuffer::create(desc);
		mLinkedListCounterParam.set(mLinkedListCounter);
	}

	void LightGridLLCreationMat::_initDefines(ShaderDefines& defines)
	{
		defines.set("THREADGROUP_SIZE", THREADGROUP_SIZE);
	}

	void LightGridLLCreationMat::setParams(const Vector3I& gridSize, const SPtr<GpuParamBlockBuffer>& gridParams,
											   const SPtr<GpuBuffer>& lightsBuffer)
	{
		mGridSize = gridSize;
		UINT32 numCells = gridSize[0] * gridSize[1] * gridSize[2];

		if(numCells > mBufferNumCells || mBufferNumCells == 0)
		{
			GPU_BUFFER_DESC desc;
			desc.elementCount = numCells;
			desc.format = BF_32X1U;
			desc.randomGpuWrite = true;
			desc.type = GBT_STANDARD;
			desc.elementSize = 0;

			mLinkedListHeads = GpuBuffer::create(desc);
			mLinkedListHeadsParam.set(mLinkedListHeads);

			desc.format = BF_32X4U;
			desc.elementCount = numCells * MAX_LIGHTS_PER_CELL;

			mLinkedList = GpuBuffer::create(desc);
			mLinkedListParam.set(mLinkedList);

			mBufferNumCells = numCells;
		}

		UINT32 zero = 0;
		mLinkedListCounter->writeData(0, sizeof(UINT32), &zero, BWT_DISCARD);

		// Note: Add a method to clear buffer data directly? e.g. GpuBuffer->clear(value);
		UINT32* headsClearData = (UINT32*)bs_stack_alloc(mLinkedListHeads->getSize());
		memset(headsClearData, 0xFFFFFFFF, mLinkedListHeads->getSize());

		mLinkedListHeads->writeData(0, mLinkedListHeads->getSize(), headsClearData, BWT_DISCARD);

		bs_stack_free(headsClearData);

		mParamsSet->setParamBlockBuffer("GridParams", gridParams, true);
		mLightBufferParam.set(lightsBuffer);
	}

	void LightGridLLCreationMat::execute(const RendererCamera& view)
	{
		mParamsSet->setParamBlockBuffer("PerCamera", view.getPerViewBuffer(), true);

		UINT32 width = view.getRenderTargets()->getWidth();
		UINT32 height = view.getRenderTargets()->getHeight();

		UINT32 numGroupsX = (mGridSize[0] + THREADGROUP_SIZE - 1) / THREADGROUP_SIZE;
		UINT32 numGroupsY = (mGridSize[1] + THREADGROUP_SIZE - 1) / THREADGROUP_SIZE;
		UINT32 numGroupsZ = (mGridSize[2] + THREADGROUP_SIZE - 1) / THREADGROUP_SIZE;

		gRendererUtility().setComputePass(mMaterial, 0);
		gRendererUtility().setPassParams(mParamsSet);

		RenderAPI::instance().dispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
	}

	void LightGridLLCreationMat::getOutputs(SPtr<GpuBuffer>& linkedListHeads, SPtr<GpuBuffer>& linkedList) const
	{
		linkedListHeads = mLinkedListHeads;
		linkedList = mLinkedList;
	}

	LightGridLLReductionMat::LightGridLLReductionMat()
		:mBufferNumCells(0)
	{
		SPtr<GpuParams> params = mParamsSet->getGpuParams();

		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gLinkedListHeads", mLinkedListHeadsParam);
		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gLinkedList", mLinkedListParam);

		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gGridDataCounter", mGridDataCounterParam);
		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gGridLightOffsetAndSize", mGridLightOffsetAndSizeParam);
		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gGridLightIndices", mGridLightIndicesParam);

		GPU_BUFFER_DESC desc;
		desc.elementCount = 1;
		desc.format = BF_32X1U;
		desc.randomGpuWrite = true;
		desc.type = GBT_STANDARD;
		desc.elementSize = 0;

		mGridDataCounter = GpuBuffer::create(desc);
		mGridDataCounterParam.set(mGridDataCounter);
	}

	void LightGridLLReductionMat::_initDefines(ShaderDefines& defines)
	{
		defines.set("THREADGROUP_SIZE", THREADGROUP_SIZE);
	}

	void LightGridLLReductionMat::setParams(const Vector3I& gridSize, const SPtr<GpuParamBlockBuffer>& gridParams,
											SPtr<GpuBuffer>& linkedListHeads, SPtr<GpuBuffer>& linkedList)
	{
		mGridSize = gridSize;
		UINT32 numCells = gridSize[0] * gridSize[1] * gridSize[2];

		if (numCells > mBufferNumCells || mBufferNumCells == 0)
		{
			GPU_BUFFER_DESC desc;
			desc.elementCount = numCells;
			desc.format = BF_32X4U;
			desc.randomGpuWrite = true;
			desc.type = GBT_STANDARD;
			desc.elementSize = 0;

			mGridLightOffsetAndSize = GpuBuffer::create(desc);
			mGridLightOffsetAndSizeParam.set(mGridLightOffsetAndSize);

			desc.format = BF_32X1U;
			desc.elementCount = numCells * MAX_LIGHTS_PER_CELL;
			mGridLightIndices = GpuBuffer::create(desc);
			mGridLightIndicesParam.set(mGridLightIndices);

			mBufferNumCells = numCells;
		}

		// Note: Add a method to clear buffer data directly? e.g. GpuBuffer->clear(value);
		UINT32 zero = 0;
		mGridDataCounter->writeData(0, sizeof(UINT32), &zero, BWT_DISCARD);

		mParamsSet->setParamBlockBuffer("GridParams", gridParams, true);
		mLinkedListHeadsParam.set(linkedListHeads);
		mLinkedListParam.set(linkedList);
	}

	void LightGridLLReductionMat::execute(const RendererCamera& view)
	{
		mParamsSet->setParamBlockBuffer("PerCamera", view.getPerViewBuffer(), true);

		UINT32 numGroupsX = (mGridSize[0] + THREADGROUP_SIZE - 1) / THREADGROUP_SIZE;
		UINT32 numGroupsY = (mGridSize[1] + THREADGROUP_SIZE - 1) / THREADGROUP_SIZE;
		UINT32 numGroupsZ = (mGridSize[2] + THREADGROUP_SIZE - 1) / THREADGROUP_SIZE;

		gRendererUtility().setComputePass(mMaterial, 0);
		gRendererUtility().setPassParams(mParamsSet);

		RenderAPI::instance().dispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
	}

	void LightGridLLReductionMat::getOutputs(SPtr<GpuBuffer>& gridOffsetsAndSize, SPtr<GpuBuffer>& gridLightIndices) const
	{
		gridOffsetsAndSize = mGridLightOffsetAndSize;
		gridLightIndices = mGridLightIndices;
	}

	LightGrid::LightGrid()
	{
		mGridParamBuffer = gLightGridParamDefDef.createBuffer();
	}

	void LightGrid::updateGrid(const RendererCamera& view, const GPULightData& lightData)
	{
		UINT32 width = view.getRenderTargets()->getWidth();
		UINT32 height = view.getRenderTargets()->getHeight();

		Vector3I gridSize;
		gridSize[0] = (width + CELL_XY_SIZE - 1) / CELL_XY_SIZE;
		gridSize[1] = (height + CELL_XY_SIZE - 1) / CELL_XY_SIZE;
		gridSize[2] = NUM_Z_SUBDIVIDES;

		Vector3I lightOffsets;
		lightOffsets[0] = lightData.getNumDirLights();
		lightOffsets[1] = lightOffsets[0] + lightData.getNumRadialLights();
		lightOffsets[2] = lightOffsets[1] + lightData.getNumSpotLights();

		UINT32 numCells = gridSize[0] * gridSize[1] * gridSize[2];

		gLightGridParamDefDef.gLightOffsets.set(mGridParamBuffer, lightOffsets);
		gLightGridParamDefDef.gNumCells.set(mGridParamBuffer, numCells);
		gLightGridParamDefDef.gGridSize.set(mGridParamBuffer, gridSize);
		gLightGridParamDefDef.gMaxNumLightsPerCell.set(mGridParamBuffer, MAX_LIGHTS_PER_CELL);
		gLightGridParamDefDef.gGridPixelSize.set(mGridParamBuffer, Vector2I(CELL_XY_SIZE, CELL_XY_SIZE));

		mLLCreationMat.setParams(gridSize, mGridParamBuffer, lightData.getLightBuffer());
		mLLCreationMat.execute(view);

		SPtr<GpuBuffer> linkedListHeads;
		SPtr<GpuBuffer> linkedList;
		mLLCreationMat.getOutputs(linkedListHeads, linkedList);

		mLLReductionMat.setParams(gridSize, mGridParamBuffer, linkedListHeads, linkedList);
		mLLReductionMat.execute(view);
	}

	void LightGrid::getOutputs(SPtr<GpuBuffer>& gridOffsetsAndSize, SPtr<GpuBuffer>& gridLightIndices, 
							   SPtr<GpuParamBlockBuffer>& gridParams) const
	{
		mLLReductionMat.getOutputs(gridOffsetsAndSize, gridLightIndices);
		gridParams = mGridParamBuffer;
	}
}}