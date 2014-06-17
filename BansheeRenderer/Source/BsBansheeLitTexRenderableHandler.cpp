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
#include "BsRenderSystem.h"

namespace BansheeEngine
{
	LitTexRenderableHandler::LitTexRenderableHandler()
	{
		defaultShader = createDefaultShader();

		TechniquePtr defaultTechnique = defaultShader->getBestTechnique();
		PassPtr defaultPass = defaultTechnique->getPass(0);

		bool matrixTranspose = defaultPass->getVertexProgram()->requiresMatrixTranspose(); // Only need this from first vertex program as this is a static setting across all GPU programs
		const GpuParamDesc& vertParamDesc = defaultPass->getVertexProgram()->getParamDesc();
		const GpuParamDesc& fragParamDesc = defaultPass->getFragmentProgram()->getParamDesc();

		GpuParamDesc staticParamsDesc;
		GpuParamDesc perFrameParamsDesc;
		GpuParamDesc perObjectParamsDesc;

		bool foundLightDir = false;
		bool foundTime = false;
		bool foundWVP = false;
		bool foundStatic = false;
		bool foundPerFrame = false;
		bool foundPerObject = false;

		const Map<String, SHADER_DATA_PARAM_DESC>& dataParams = defaultShader->_getDataParams();
		for (auto& param : dataParams)
		{
			if (!foundLightDir && param.second.rendererSemantic == RPS_LightDir)
			{
				auto iterFind = fragParamDesc.params.find(param.second.gpuVariableName);
				if (iterFind == fragParamDesc.params.end())
					continue;

				lightDirParamDesc = iterFind->second;
				staticParamsDesc.params[iterFind->first] = iterFind->second;
				foundLightDir = true;
			}
			else if (!foundTime && param.second.rendererSemantic == RPS_Time)
			{
				auto iterFind = vertParamDesc.params.find(param.second.gpuVariableName);
				if (iterFind == vertParamDesc.params.end())
					continue;

				timeParamDesc = iterFind->second;
				perFrameParamsDesc.params[iterFind->first] = iterFind->second;
				foundTime = true;
			}
			else if (!foundWVP && param.second.rendererSemantic == RPS_WorldViewProjTfrm)
			{
				auto iterFind = vertParamDesc.params.find(param.second.gpuVariableName);
				if (iterFind == vertParamDesc.params.end())
					continue;

				wvpParamDesc = iterFind->second;
				perObjectParamsDesc.params[iterFind->first] = iterFind->second;
				foundWVP = true;
			}
		}

		const Map<String, SHADER_PARAM_BLOCK_DESC>& paramBlocks = defaultShader->_getParamBlocks();
		for (auto& block : paramBlocks)
		{
			if (!foundStatic && block.second.rendererSemantic == RBS_Static)
			{
				auto iterFind = fragParamDesc.paramBlocks.find(block.second.name);
				if (iterFind == fragParamDesc.paramBlocks.end())
					continue;

				staticParamBlockDesc = iterFind->second;
				staticParamsDesc.paramBlocks[iterFind->first] = iterFind->second;
				foundStatic = true;
			}
			else if (!foundPerFrame && block.second.rendererSemantic == RBS_PerFrame)
			{
				auto iterFind = vertParamDesc.paramBlocks.find(block.second.name);
				if (iterFind == vertParamDesc.paramBlocks.end())
					continue;

				perFrameParamBlockDesc = iterFind->second;
				perFrameParamsDesc.paramBlocks[iterFind->first] = iterFind->second;
				foundPerFrame = true;
			}
			else if (!foundPerObject && block.second.rendererSemantic == RBS_PerObject)
			{
				auto iterFind = vertParamDesc.paramBlocks.find(block.second.name);
				if (iterFind == vertParamDesc.paramBlocks.end())
					continue;

				perObjectParamBlockDesc = iterFind->second;
				perObjectParamsDesc.paramBlocks[iterFind->first] = iterFind->second;
				foundPerObject = true;
			}
		}

		if (!foundLightDir || !foundLightDir || !foundWVP || !foundStatic || !foundPerFrame || !foundPerObject)
			BS_EXCEPT(InternalErrorException, "Invalid default shader.");

		// Create global GPU param buffers and get parameter handles
		staticParams = bs_shared_ptr<GpuParams>(staticParamsDesc, matrixTranspose);
		perFrameParams = bs_shared_ptr<GpuParams>(perFrameParamsDesc, matrixTranspose);

		staticParamBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(staticParamBlockDesc.blockSize * sizeof(UINT32));
		perFrameParamBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(perFrameParamBlockDesc.blockSize * sizeof(UINT32));

		staticParams->setParamBlockBuffer(staticParamBlockDesc.slot, staticParamBuffer);
		perFrameParams->setParamBlockBuffer(perFrameParamBlockDesc.slot, perFrameParamBuffer);

		staticParams->getParam(lightDirParamDesc.name, lightDirParam);
		perFrameParams->getParam(timeParamDesc.name, timeParam);

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
								rendererData->perObjectParamBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(perObjectParamBlockDesc.blockSize * sizeof(UINT32));

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

	ShaderPtr LitTexRenderableHandler::createDefaultShader()
	{
		String rsName = RenderSystem::instance().getName();

		HGpuProgram vsProgram;
		HGpuProgram psProgram;

		if (rsName == RenderSystemDX11)
		{
			String vsCode = R"(
			cbuffer PerFrame
			{
				float time;
			}
				
			cbuffer PerObject
			{
				float4x4 matWorldViewProj;
			}

			void vs_main(in float3 inPos : POSITION,
					     out float4 oPosition : SV_Position)
			{
				 oPosition = mul(matWorldViewProj, float4(inPos.xyz + float3(sin(time), 0, 0), 1));
			})";

			String psCode = R"(
			cbuffer Static
			{
				float4 lightDir;
			}
			
			float4 ps_main() : SV_Target
			{
				return dot(lightDir, float4(0.5f, 0.5f, 0.5f, 0.5f));
			})";	

			vsProgram = GpuProgram::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
			psProgram = GpuProgram::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);
		}
		else if (rsName == RenderSystemDX9)
		{
			String vsCode = R"(
			BS_PARAM_BLOCK PerFrame { time }
			BS_PARAM_BLOCK PerObject { matWorldViewProj }

			float time;
			float4x4 matWorldViewProj;

			void vs_main(in float3 inPos : POSITION,
						out float4 oPosition : POSITION)
			{
				oPosition = mul(matWorldViewProj, float4(inPos.xyz + float3(sin(time), 0, 0), 1));
			})";

			String psCode = R"(
			BS_PARAM_BLOCK Static { lightDir }

			float4 lightDir;

			float4 ps_main() : COLOR0
			{
				return dot(lightDir, float4(0.5f, 0.5f, 0.5f, 0.5f));
			})";

			vsProgram = GpuProgram::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
			psProgram = GpuProgram::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);
		}
		else if (rsName == RenderSystemOpenGL)
		{
			String vsCode = R"(
			#version 400\n

			uniform PerFrame
			{
				float time;
			}

			uniform PerObject
			{
				mat4 matWorldViewProj;
			}

			in vec3 bs_position;

			void main()
			{
				gl_Position = matWorldViewProj * vec4(bs_position.xyz + vec3(sin(time), 0, 0), 1);
			})";

			String psCode = R"(
			#version 400\n

			uniform Static
			{
				vec4 lightDir;
			}

			out vec4 fragColor;

			void main()
			{
				fragColor = dot(lightDir, vec4(0.5f, 0.5f, 0.5f, 0.5f));
			})";

			vsProgram = GpuProgram::create(vsCode, "vs_main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
			psProgram = GpuProgram::create(psCode, "ps_main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);
		}

		vsProgram.synchronize();
		psProgram.synchronize();

		ShaderPtr defaultShader = Shader::create("LitTexDefault");
		defaultShader->setParamBlockAttribs("Static", true, GPBU_DYNAMIC, RBS_Static);
		defaultShader->setParamBlockAttribs("PerFrame", true, GPBU_DYNAMIC, RBS_PerFrame);
		defaultShader->setParamBlockAttribs("PerObject", true, GPBU_DYNAMIC, RBS_PerObject);

		defaultShader->addParameter("lightDir", "lightDir", GPDT_FLOAT4, RPS_LightDir);
		defaultShader->addParameter("time", "time", GPDT_FLOAT1, RPS_Time);
		defaultShader->addParameter("matWorldViewProj", "matWorldViewProj", GPDT_MATRIX_4X4, RPS_WorldViewProjTfrm);

		TechniquePtr newTechnique = defaultShader->addTechnique(rsName, RendererDefault);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		return defaultShader;
	}
}