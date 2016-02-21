//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsStaticRenderableHandler.h"
#include "BsShader.h"
#include "BsGpuParams.h"
#include "BsRenderBeast.h"
#include "BsMaterial.h"

namespace BansheeEngine
{
	StaticRenderableHandler::StaticRenderableHandler()
	{ }

	void StaticRenderableHandler::initializeRenderElem(RenderableElement& element)
	{
		element.rendererData = PerObjectData();
		PerObjectData* rendererData = any_cast_unsafe<PerObjectData>(&element.rendererData);

		SPtr<ShaderCore> shader = element.material->getShader();
		if (shader == nullptr)
		{
			LOGWRN("Missing shader on material.");
			return;
		}

		const Map<String, SHADER_PARAM_BLOCK_DESC>& paramBlockDescs = shader->getParamBlocks();
		String perFrameBlockName;
		String perCameraBlockName;
		String perObjectBlockName;

		for (auto& paramBlockDesc : paramBlockDescs)
		{
			if (paramBlockDesc.second.rendererSemantic == RBS_PerFrame)
				perFrameBlockName = paramBlockDesc.second.name;
			else if (paramBlockDesc.second.rendererSemantic == RBS_PerCamera)
				perCameraBlockName = paramBlockDesc.second.name;
			else if (paramBlockDesc.second.rendererSemantic == RBS_PerObject)
				perObjectBlockName = paramBlockDesc.second.name;
		}

		UINT32 numPasses = element.material->getNumPasses();
		for (UINT32 i = 0; i < numPasses; i++)
		{
			SPtr<PassParametersCore> passParams = element.material->getPassParameters(i);

			for (UINT32 j = 0; j < PassParametersCore::NUM_PARAMS; j++)
			{
				SPtr<GpuParamsCore> gpuParams = passParams->getParamByIdx(j);
				if (gpuParams == nullptr)
					continue;

				const GpuParamDesc& paramsDesc = gpuParams->getParamDesc();

				if (perFrameBlockName != "")
				{
					auto findIter = paramsDesc.paramBlocks.find(perFrameBlockName);
					if (findIter != paramsDesc.paramBlocks.end())
					{
						// TODO - We only compare block sizes but not actual contents. Should I check them too?
						//        Probably shouldn't concern myself with that here, instead check that on a higher level.
						if (findIter->second.blockSize == mPerFrameParams.getDesc().blockSize)
						{
							UINT32 slotIdx = findIter->second.slot;
							element.rendererBuffers.push_back(RenderableElement::BufferBindInfo(i, j, slotIdx, mPerFrameParams.getBuffer()));
						}
					}
				}

				if (perCameraBlockName != "")
				{
					auto findIter = paramsDesc.paramBlocks.find(perCameraBlockName);
					if (findIter != paramsDesc.paramBlocks.end())
					{
						if (findIter->second.blockSize == mPerCameraParams.getDesc().blockSize)
						{
							UINT32 slotIdx = findIter->second.slot;
							element.rendererBuffers.push_back(RenderableElement::BufferBindInfo(i, j, slotIdx, mPerCameraParams.getBuffer()));
						}
					}
				}

				if (perObjectBlockName != "")
				{
					auto findIter = paramsDesc.paramBlocks.find(perObjectBlockName);
					if (findIter != paramsDesc.paramBlocks.end())
					{
						if (findIter->second.blockSize == mPerObjectParams.getDesc().blockSize)
						{
							UINT32 slotIdx = findIter->second.slot;
							rendererData->perObjectBuffers.push_back(RenderableElement::BufferBindInfo(i, j, slotIdx, mPerObjectParams.getBuffer()));
						}
					}
				}
			}
		}
	}

	void StaticRenderableHandler::bindPerObjectBuffers(const RenderableElement& element)
	{
		const PerObjectData* rendererData = any_cast_unsafe<PerObjectData>(&element.rendererData);
		for (auto& perObjectBuffer : rendererData->perObjectBuffers)
		{
			SPtr<GpuParamsCore> params = element.material->getPassParameters(perObjectBuffer.passIdx)->getParamByIdx(perObjectBuffer.paramsIdx);

			params->setParamBlockBuffer(perObjectBuffer.slotIdx, perObjectBuffer.buffer);
		}
	}

	void StaticRenderableHandler::updatePerFrameBuffers(float time)
	{
		mPerFrameParams.gTime.set(time);
	}

	void StaticRenderableHandler::updatePerCameraBuffers(const CameraShaderData& cameraData)
	{
		mPerCameraParams.gViewDir.set(cameraData.viewDir);
		mPerCameraParams.gViewOrigin.set(cameraData.viewOrigin);
		mPerCameraParams.gMatView.set(cameraData.view);
		mPerCameraParams.gMatProj.set(cameraData.proj);
		mPerCameraParams.gMatViewProj.set(cameraData.viewProj);
		mPerCameraParams.gMatInvProj.set(cameraData.invProj);
		mPerCameraParams.gMatInvViewProj.set(cameraData.invViewProj);
		mPerCameraParams.gMatScreenToWorld.set(cameraData.screenToWorld);
		mPerCameraParams.gDeviceZToWorldZ.set(cameraData.deviceZToWorldZ);
		mPerCameraParams.gClipToUVScaleOffset.set(cameraData.clipToUVScaleOffset);
	}

	void StaticRenderableHandler::updatePerObjectBuffers(RenderableElement& element, const RenderableShaderData& data, const Matrix4& wvpMatrix)
	{
		// TODO - If I kept all the values in the same structure maybe a simple memcpy directly into the constant buffer would be better (i.e. faster)?
		mPerObjectParams.gMatWorld.set(data.worldTransform);
		mPerObjectParams.gMatInvWorld.set(data.invWorldTransform);
		mPerObjectParams.gMatWorldNoScale.set(data.worldNoScaleTransform);
		mPerObjectParams.gMatInvWorldNoScale.set(data.invWorldNoScaleTransform);
		mPerObjectParams.gWorldDeterminantSign.set(data.worldDeterminantSign);
		mPerObjectParams.gMatWorldViewProj.set(wvpMatrix);
	}
}