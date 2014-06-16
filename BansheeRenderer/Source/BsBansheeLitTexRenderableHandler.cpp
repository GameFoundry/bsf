#include "BsBansheeLitTexRenderableHandler.h"
#include "BsRenderableProxy.h"
#include "BsShader.h"
#include "BsShaderProxy.h"
#include "BsGpuParams.h"
#include "BsBansheeRenderer.h"
#include "BsHardwareBufferManager.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsTechnique.h"
#include "BsPass.h"

namespace BansheeEngine
{
	LitTexRenderableHandler::LitTexRenderableHandler()
	{
		defaultShader = nullptr; // TODO - Get proper shader

		TechniquePtr defaultTechnique = defaultShader->getBestTechnique();
		PassPtr defaultPass = defaultTechnique->getPass(0);

		bool matrixTranspose = defaultPass->getVertexProgram()->requiresMatrixTranspose(); // This is a static setting across all GPU programs
		const GpuParamDesc& vertParamDesc = defaultPass->getVertexProgram()->getParamDesc();
		const GpuParamDesc& fragParamDesc = defaultPass->getFragmentProgram()->getParamDesc();

		GpuParamDesc staticParamsDesc;
		GpuParamDesc perFrameParamsDesc;
		GpuParamDesc perObjectParamsDesc;

		const Map<String, SHADER_DATA_PARAM_DESC>& dataParams = defaultShader->_getDataParams();
		for (auto& param : dataParams)
		{
			if (param.second.rendererSemantic == RPS_LightDir)
			{
				auto iterFind = fragParamDesc.params.find(param.second.gpuVariableName);
				if (iterFind == fragParamDesc.params.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				lightDirParamDesc = iterFind->second;
				staticParamsDesc.params[iterFind->first] = iterFind->second;
			}
			else if (param.second.rendererSemantic == RPS_Time)
			{
				auto iterFind = vertParamDesc.params.find(param.second.gpuVariableName);
				if (iterFind == vertParamDesc.params.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				timeParamDesc = iterFind->second;
				perFrameParamsDesc.params[iterFind->first] = iterFind->second;
			}
			else if (param.second.rendererSemantic == RPS_WorldViewProjTfrm)
			{
				auto iterFind = vertParamDesc.params.find(param.second.gpuVariableName);
				if (iterFind == vertParamDesc.params.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				wvpParamDesc = iterFind->second;
				perObjectParamsDesc.params[iterFind->first] = iterFind->second;
			}
		}

		const Map<String, SHADER_PARAM_BLOCK_DESC>& paramBlocks = defaultShader->_getParamBlocks();
		for (auto& block : paramBlocks)
		{
			if (block.second.rendererSemantic == RBS_Static)
			{
				auto iterFind = vertParamDesc.paramBlocks.find(block.second.name);
				if (iterFind == vertParamDesc.paramBlocks.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				staticParamBlockDesc = iterFind->second;
				staticParamsDesc.paramBlocks[iterFind->first] = iterFind->second;
			}
			else if (block.second.rendererSemantic == RBS_PerFrame)
			{
				auto iterFind = fragParamDesc.paramBlocks.find(block.second.name);
				if (iterFind == fragParamDesc.paramBlocks.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				perFrameParamBlockDesc = iterFind->second;
				perFrameParamsDesc.paramBlocks[iterFind->first] = iterFind->second;
			}
			else if (block.second.rendererSemantic == RBS_PerObject)
			{
				auto iterFind = vertParamDesc.paramBlocks.find(block.second.name);
				if (iterFind == vertParamDesc.paramBlocks.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				perObjectParamBlockDesc = iterFind->second;
				perObjectParamsDesc.paramBlocks[iterFind->first] = iterFind->second;
			}
		}

		// Create global GPU param buffers and get parameter handles
		staticParams = bs_shared_ptr<GpuParams>(staticParamsDesc, matrixTranspose);
		perFrameParams = bs_shared_ptr<GpuParams>(perFrameParamsDesc, matrixTranspose);

		staticParamBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(staticParamBlockDesc.blockSize);
		perFrameParamBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(perFrameParamBlockDesc.blockSize);

		staticParams->setParamBlockBuffer(staticParamBlockDesc.slot, staticParamBuffer);
		perFrameParams->setParamBlockBuffer(perFrameParamBlockDesc.slot, perFrameParamBuffer);

		staticParams->getParam(lightDirParamDesc.name, lightDirParam);
		staticParams->getParam(timeParamDesc.name, timeParam);

		lightDirParam.set(Vector4(0.707f, 0.707f, 0.707f, 0.0f));
	}

	void LitTexRenderableHandler::initializeProxy(const RenderableProxyPtr& proxy)
	{
		static auto paramsMatch = [](const GpuParamDataDesc& a, const GpuParamDataDesc& b)
		{
			return a.gpuMemOffset == b.gpuMemOffset && a.elementSize == b.elementSize &&
				a.arraySize == b.arraySize && a.arrayElementStride == b.arrayElementStride;
		};

		for (auto& element : proxy->renderableElements)
		{
			element->rendererData = PerObjectData();
			PerObjectData* rendererData = any_cast_unsafe<PerObjectData>(&element->rendererData);

			ShaderProxyPtr shader = element->material->shader;

			const Map<String, SHADER_PARAM_BLOCK_DESC>& paramBlockDescs = shader->paramBlocks;
			const Map<String, SHADER_DATA_PARAM_DESC>& dataParamDescs = shader->dataParams;
			String staticBlockName;
			String perFrameBlockName;
			String perObjectBlockName;

			String wvpParamName;

			for (auto& paramBlockDesc : paramBlockDescs)
			{
				switch (paramBlockDesc.second.rendererSemantic)
				{
				case RBS_Static:
					staticBlockName = paramBlockDesc.second.name;
					break;
				case RBS_PerFrame:
					perFrameBlockName = paramBlockDesc.second.name;
					break;
				case RBS_PerObject:
					perObjectBlockName = paramBlockDesc.second.name;
					break;
				}
			}

			for (auto& paramDesc : dataParamDescs)
			{
				if (paramDesc.second.rendererSemantic == RPS_WorldViewProjTfrm)
					wvpParamName = paramDesc.second.gpuVariableName;
			}

			UINT32 idx = 0;
			for (auto& gpuParams : element->material->params)
			{
				const GpuParamDesc& paramsDesc = gpuParams->getParamDesc();

				if (staticBlockName != "")
				{
					auto findIter = paramsDesc.paramBlocks.find(staticBlockName);
					if (findIter != paramsDesc.paramBlocks.end())
					{
						// TODO - We only compare block sizes but not actual contents. Should I check them too?
						//        Probably shouldn't concern myself with that here, instead check that on a higher level.
						if (findIter->second.blockSize == staticParamBlockDesc.blockSize)
						{
							UINT32 slotIdx = findIter->second.slot;
							element->material->rendererBuffers.push_back(MaterialProxy::BufferBindInfo(idx, slotIdx, staticParamBuffer));
						}
					}
				}

				if (perFrameBlockName != "")
				{
					auto findIter = paramsDesc.paramBlocks.find(perFrameBlockName);
					if (findIter != paramsDesc.paramBlocks.end())
					{
						if (findIter->second.blockSize == perFrameParamBlockDesc.blockSize)
						{
							UINT32 slotIdx = findIter->second.slot;
							element->material->rendererBuffers.push_back(MaterialProxy::BufferBindInfo(idx, slotIdx, perFrameParamBuffer));
						}
					}
				}

				if (perObjectBlockName != "")
				{
					auto findIter = paramsDesc.paramBlocks.find(perObjectBlockName);
					if (findIter != paramsDesc.paramBlocks.end())
					{
						if (findIter->second.blockSize == perObjectParamBlockDesc.blockSize)
						{
							if (rendererData->perObjectParamBuffer == nullptr)
								rendererData->perObjectParamBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(perObjectParamBlockDesc.blockSize);

							rendererData->perObjectBuffers.push_back(MaterialProxy::BufferBindInfo(idx, findIter->second.slot, rendererData->perObjectParamBuffer));

							if (!rendererData->hasWVPParam && wvpParamName != "")
							{
								auto findIter2 = paramsDesc.params.find(wvpParamName);
								if (findIter2 != paramsDesc.params.end())
								{
									if (paramsMatch(findIter2->second, wvpParamDesc))
									{
										gpuParams->getParam(wvpParamName, rendererData->wvpParam);
										rendererData->hasWVPParam = true;
									}
								}
							}
						}
					}
				}

				idx++;
			}

			bindGlobalBuffers(element);
		}
	}

	void LitTexRenderableHandler::bindPerObjectBuffers(const RenderableProxyPtr& renderable, const RenderableElement* element)
	{
		if (renderable->renderableType != RenType_LitTextured)
			return;

		const PerObjectData* rendererData = any_cast_unsafe<PerObjectData>(&element->rendererData);
		for (auto& perObjectBuffer : rendererData->perObjectBuffers)
		{
			GpuParamsPtr params = element->material->params[perObjectBuffer.paramsIdx];

			params->setParamBlockBuffer(perObjectBuffer.slotIdx, rendererData->perObjectParamBuffer);
		}
	}

	void LitTexRenderableHandler::updateGlobalBuffers(float time)
	{
		timeParam.set(time);

		staticParams->updateHardwareBuffers();
		perFrameParams->updateHardwareBuffers();
	}

	void LitTexRenderableHandler::updatePerObjectBuffers(RenderableElement* element, const Matrix4& wvpMatrix)
	{
		PerObjectData* rendererData = any_cast_unsafe<PerObjectData>(&element->rendererData);

		if (rendererData->hasWVPParam)
			rendererData->wvpParam.set(wvpMatrix);

		if (rendererData->perObjectParamBuffer != nullptr)
		{
			GpuParamBlockPtr paramBlock = rendererData->perObjectParamBuffer->getParamBlock();
			if (paramBlock->isDirty())
				paramBlock->uploadToBuffer(rendererData->perObjectParamBuffer);
		}
	}
}