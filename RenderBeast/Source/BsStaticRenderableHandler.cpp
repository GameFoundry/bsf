#include "BsStaticRenderableHandler.h"
#include "BsShader.h"
#include "BsGpuParams.h"
#include "BsRenderBeast.h"
#include "BsHardwareBufferManager.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsMaterial.h"
#include "BsRenderAPI.h"

namespace BansheeEngine
{
	StaticRenderableHandler::StaticRenderableHandler()
	{
		defaultShader = createDefaultShader();

		SPtr<TechniqueCore> defaultTechnique = defaultShader->getBestTechnique();
		SPtr<PassCore> defaultPass = defaultTechnique->getPass(0);

		GpuParamDescPtr vertParamDesc = defaultPass->getVertexProgram()->getParamDesc();
		GpuParamDescPtr fragParamDesc = defaultPass->getFragmentProgram()->getParamDesc();

		GpuParamDescPtr perFrameParamsDesc = bs_shared_ptr_new<GpuParamDesc>();
		GpuParamDescPtr perObjectParamsDesc = bs_shared_ptr_new<GpuParamDesc>();
		GpuParamDescPtr perCameraParamsDesc = bs_shared_ptr_new<GpuParamDesc>();

		bool foundTime = false;
		bool foundWVP = false;
		bool foundVP = false;
		bool foundW = false;
		bool foundV = false;
		bool foundP = false;
		bool foundViewDir = false;
		bool foundPerFrame = false;
		bool foundPerObject = false;
		bool foundPerCamera = false;

		const Map<String, SHADER_DATA_PARAM_DESC>& dataParams = defaultShader->getDataParams();
		for (auto& param : dataParams)
		{
			if (!foundTime && param.second.rendererSemantic == RPS_Time)
			{
				auto iterFind = vertParamDesc->params.find(param.second.gpuVariableName);
				if (iterFind == vertParamDesc->params.end())
					continue;

				timeParamDesc = iterFind->second;
				perFrameParamsDesc->params[iterFind->first] = iterFind->second;
				foundTime = true;
			}
			else if (!foundWVP && param.second.rendererSemantic == RPS_WorldViewProjTfrm)
			{
				auto iterFind = vertParamDesc->params.find(param.second.gpuVariableName);
				if (iterFind == vertParamDesc->params.end())
					continue;

				wvpParamDesc = iterFind->second;
				perObjectParamsDesc->params[iterFind->first] = iterFind->second;
				foundWVP = true;
			}
			else if (!foundW && param.second.rendererSemantic == RPS_WorldTfrm)
			{
				auto iterFind = vertParamDesc->params.find(param.second.gpuVariableName);
				if (iterFind == vertParamDesc->params.end())
					continue;

				wParamDesc = iterFind->second;
				perObjectParamsDesc->params[iterFind->first] = iterFind->second;
				foundW = true;
			}
			else if (!foundVP && param.second.rendererSemantic == RPS_ViewProjTfrm)
			{
				auto iterFind = fragParamDesc->params.find(param.second.gpuVariableName);
				if (iterFind == fragParamDesc->params.end())
					continue;

				vpParamDesc = iterFind->second;
				perCameraParamsDesc->params[iterFind->first] = iterFind->second;
				foundVP = true;
			}
			else if (!foundV && param.second.rendererSemantic == RPS_ViewTfrm)
			{
				auto iterFind = fragParamDesc->params.find(param.second.gpuVariableName);
				if (iterFind == fragParamDesc->params.end())
					continue;

				vParamDesc = iterFind->second;
				perCameraParamsDesc->params[iterFind->first] = iterFind->second;
				foundV = true;
			}
			else if (!foundP && param.second.rendererSemantic == RPS_ProjTfrm)
			{
				auto iterFind = fragParamDesc->params.find(param.second.gpuVariableName);
				if (iterFind == fragParamDesc->params.end())
					continue;

				pParamDesc = iterFind->second;
				perCameraParamsDesc->params[iterFind->first] = iterFind->second;
				foundP = true;
			}
			else if (!foundViewDir && param.second.rendererSemantic == RPS_ViewDir)
			{
				auto iterFind = fragParamDesc->params.find(param.second.gpuVariableName);
				if (iterFind == fragParamDesc->params.end())
					continue;

				viewDirParamDesc = iterFind->second;
				perCameraParamsDesc->params[iterFind->first] = iterFind->second;
				foundViewDir = true;
			}
		}

		const Map<String, SHADER_PARAM_BLOCK_DESC>& paramBlocks = defaultShader->getParamBlocks();
		for (auto& block : paramBlocks)
		{
			if (!foundPerFrame && block.second.rendererSemantic == RBS_PerFrame)
			{
				auto iterFind = vertParamDesc->paramBlocks.find(block.second.name);
				if (iterFind == vertParamDesc->paramBlocks.end())
					continue;

				perFrameParamBlockDesc = iterFind->second;
				perFrameParamsDesc->paramBlocks[iterFind->first] = iterFind->second;
				foundPerFrame = true;
			}
			else if (!foundPerCamera && block.second.rendererSemantic == RBS_PerCamera)
			{
				auto iterFind = fragParamDesc->paramBlocks.find(block.second.name);
				if (iterFind == fragParamDesc->paramBlocks.end())
					continue;

				perCameraParamBlockDesc = iterFind->second;
				perCameraParamsDesc->paramBlocks[iterFind->first] = iterFind->second;
				foundPerCamera = true;
			}
			else if (!foundPerObject && block.second.rendererSemantic == RBS_PerObject)
			{
				auto iterFind = vertParamDesc->paramBlocks.find(block.second.name);
				if (iterFind == vertParamDesc->paramBlocks.end())
					continue;

				perObjectParamBlockDesc = iterFind->second;
				perObjectParamsDesc->paramBlocks[iterFind->first] = iterFind->second;
				foundPerObject = true;
			}
		}

		if (!foundTime || !foundWVP || !foundVP || !foundW || !foundV || !foundP || !foundViewDir || !foundPerFrame || !foundPerCamera || !foundPerObject)
			BS_EXCEPT(InternalErrorException, "Invalid default shader.");

		// Create global GPU param buffers and get parameter handles
		perFrameParams = GpuParamsCore::create(perFrameParamsDesc, false);
		perCameraParams = GpuParamsCore::create(perCameraParamsDesc, false);

		perFrameParamBuffer = HardwareBufferCoreManager::instance().createGpuParamBlockBuffer(perFrameParamBlockDesc.blockSize * sizeof(UINT32));
		perCameraParamBuffer = HardwareBufferCoreManager::instance().createGpuParamBlockBuffer(perCameraParamBlockDesc.blockSize * sizeof(UINT32));
		perObjectParamBuffer = HardwareBufferCoreManager::instance().createGpuParamBlockBuffer(perObjectParamBlockDesc.blockSize * sizeof(UINT32));

		perFrameParams->setParamBlockBuffer(perFrameParamBlockDesc.slot, perFrameParamBuffer);
		perCameraParams->setParamBlockBuffer(perCameraParamBlockDesc.slot, perCameraParamBuffer);

		perFrameParams->getParam(timeParamDesc.name, timeParam);
		perCameraParams->getParam(viewDirParamDesc.name, viewDirParam);
	}

	void StaticRenderableHandler::initializeRenderElem(RenderableElement& element)
	{
		static auto paramsMatch = [](const GpuParamDataDesc& a, const GpuParamDataDesc& b)
		{
			return a.gpuMemOffset == b.gpuMemOffset && a.elementSize == b.elementSize &&
				a.arraySize == b.arraySize && a.arrayElementStride == b.arrayElementStride;
		};

		element.rendererData = PerObjectData();
		PerObjectData* rendererData = any_cast_unsafe<PerObjectData>(&element.rendererData);

		SPtr<ShaderCore> shader = element.material->getShader();
		if (shader == nullptr)
		{
			LOGWRN("Missing shader on material.");
			return;
		}

		const Map<String, SHADER_PARAM_BLOCK_DESC>& paramBlockDescs = shader->getParamBlocks();
		const Map<String, SHADER_DATA_PARAM_DESC>& dataParamDescs = shader->getDataParams();
		String perFrameBlockName;
		String perCameraBlockName;
		String perObjectBlockName;

		String wvpParamName;
		String wParamName;

		for (auto& paramBlockDesc : paramBlockDescs)
		{
			if (paramBlockDesc.second.rendererSemantic == RBS_PerFrame)
				perFrameBlockName = paramBlockDesc.second.name;
			else if (paramBlockDesc.second.rendererSemantic == RBS_PerCamera)
				perCameraBlockName = paramBlockDesc.second.name;
			else if (paramBlockDesc.second.rendererSemantic == RBS_PerObject)
				perObjectBlockName = paramBlockDesc.second.name;
		}

		for (auto& paramDesc : dataParamDescs)
		{
			if (paramDesc.second.rendererSemantic == RPS_WorldViewProjTfrm)
				wvpParamName = paramDesc.second.gpuVariableName;
			else if (paramDesc.second.rendererSemantic == RPS_WorldTfrm)
				wParamName = paramDesc.second.gpuVariableName;
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
						if (findIter->second.blockSize == perFrameParamBlockDesc.blockSize)
						{
							UINT32 slotIdx = findIter->second.slot;
							element.rendererBuffers.push_back(RenderableElement::BufferBindInfo(i, j, slotIdx, perFrameParamBuffer));
						}
					}
				}

				if (perCameraBlockName != "")
				{
					auto findIter = paramsDesc.paramBlocks.find(perCameraBlockName);
					if (findIter != paramsDesc.paramBlocks.end())
					{
						if (findIter->second.blockSize == perCameraParamBlockDesc.blockSize)
						{
							UINT32 slotIdx = findIter->second.slot;
							element.rendererBuffers.push_back(RenderableElement::BufferBindInfo(i, j, slotIdx, perCameraParamBuffer));
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
							rendererData->perObjectBuffers.push_back(RenderableElement::BufferBindInfo(i, j, findIter->second.slot, perObjectParamBuffer));

							if (rendererData->wvpParam == nullptr && wvpParamName != "")
							{
								auto findIter2 = paramsDesc.params.find(wvpParamName);
								if (findIter2 != paramsDesc.params.end())
								{
									if (paramsMatch(findIter2->second, wvpParamDesc))
										gpuParams->getParam(wvpParamName, rendererData->wvpParam);
								}
							}

							if (rendererData->wParam == nullptr && wParamName != "")
							{
								auto findIter2 = paramsDesc.params.find(wParamName);
								if (findIter2 != paramsDesc.params.end())
								{
									if (paramsMatch(findIter2->second, wParamDesc))
										gpuParams->getParam(wParamName, rendererData->wParam);
								}
							}
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

			params->setParamBlockBuffer(perObjectBuffer.slotIdx, perObjectParamBuffer);
		}
	}

	void StaticRenderableHandler::updatePerFrameBuffers(float time)
	{
		timeParam.set(time);

		perFrameParams->updateHardwareBuffers();
	}

	void StaticRenderableHandler::updatePerCameraBuffers(const Matrix4& vpMatrix, const Matrix4& vMatrix, const Matrix4& pMatrix, const Vector3& viewDir)
	{
		viewProjMatParam.set(vpMatrix);
		viewMatParam.set(vMatrix);
		projMatParam.set(pMatrix);
		viewDirParam.set(viewDir);

		perCameraParams->updateHardwareBuffers();
	}

	void StaticRenderableHandler::updatePerObjectBuffers(RenderableElement& element, const Matrix4& worldMatrix, const Matrix4& wvpMatrix)
	{
		PerObjectData* rendererData = any_cast_unsafe<PerObjectData>(&element.rendererData);

		rendererData->wParam.set(worldMatrix);
		rendererData->wvpParam.set(wvpMatrix);
	}

	SPtr<ShaderCore> StaticRenderableHandler::createDefaultShader()
	{
		StringID rsName = RenderAPICore::instance().getName();

		SPtr<GpuProgramCore> vsProgram;
		SPtr<GpuProgramCore> psProgram;

		if (rsName == RenderAPIDX11)
		{
			String vsCode = R"(
			cbuffer PerFrame
			{
				float gTime;
			}
				
			cbuffer PerObject
			{
				float4x4 gMatWorldViewProj;
				float4x4 gMatWorld;
			}

			void vs_main(in float3 inPos : POSITION,
					     out float4 oPosition : SV_Position)
			{
				 oPosition = mul(gMatWorldViewProj, float4(inPos.xyz + float3(sin(gTime), 0, 0), 1));
			})";

			String psCode = R"(
			cbuffer PerCamera
			{
				float3 gViewDir;
				float4x4 gMatViewProj;
				float4x4 gMatView;
				float4x4 gMatProj;
			}
			
			float4 ps_main() : SV_Target
			{
				return dot(gViewDir, float4(0.5f, 0.5f, 0.5f, 0.5f));
			})";	

			vsProgram = GpuProgramCore::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
			psProgram = GpuProgramCore::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_FS_4_0);
		}
		else if (rsName == RenderAPIDX9)
		{
			String vsCode = R"(
			BS_PARAM_BLOCK PerFrame { gTime }
			BS_PARAM_BLOCK PerObject { gMatWorldViewProj, gMatWorld }

			float gTime;
			float4x4 gMatWorldViewProj;
			float4x4 gMatWorld;

			void vs_main(in float3 inPos : POSITION,
						out float4 oPosition : POSITION)
			{
				oPosition = mul(mul(gMatWorld, gMatWorldViewProj), float4(inPos.xyz + float3(sin(gTime), 0, 0), 1));
			})";

			String psCode = R"(
			BS_PARAM_BLOCK PerCamera { gViewDir, gMatViewProj, gMatView, gMatProj }

			float3 gViewDir;
			float4x4 gMatViewProj;
			float4x4 gMatView;
			float4x4 gMatProj;

			float4 ps_main() : COLOR0
			{
				float4x4 dummy = gMatViewProj * gMatView * gMatProj;

				return dot(mul(dummy, float4(gViewDir, 1.0f)), float4(0.5f, 0.5f, 0.5f, 0.5f));
			})";

			vsProgram = GpuProgramCore::create(vsCode, "vs_main", "hlsl9", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
			psProgram = GpuProgramCore::create(psCode, "ps_main", "hlsl9", GPT_FRAGMENT_PROGRAM, GPP_FS_2_0);
		}
		else if (rsName == RenderAPIOpenGL)
		{
			String vsCode = R"(
			uniform PerFrame
			{
				float gTime;
			};

			uniform PerObject
			{
				mat4 gMatWorldViewProj;
				mat4 gMatWorld;
			};

			in vec3 bs_position;

			out gl_PerVertex
			{
				vec4 gl_Position;
			};

			void main()
			{
				gl_Position = gMatWorld * gMatWorldViewProj * vec4(bs_position.xyz + vec3(sin(gTime), 0, 0), 1);
			})";

			String psCode = R"(
			uniform PerCamera
			{
				vec3 gViewDir;
				mat4 gMatViewProj;
				mat4 gMatView;
				mat4 gMatProj;
			};

			out vec4 fragColor;

			void main()
			{
				fragColor.x = dot(gMatViewProj * gMatView * gMatProj * vec4(gViewDir, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1.0f));
			})";

			vsProgram = GpuProgramCore::create(vsCode, "main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
			psProgram = GpuProgramCore::create(psCode, "main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_FS_4_0);
		}

		PASS_DESC_CORE passDesc;
		passDesc.vertexProgram = vsProgram;
		passDesc.fragmentProgram = psProgram;

		SPtr<PassCore> newPass = PassCore::create(passDesc);
		SPtr<TechniqueCore> newTechnique = TechniqueCore::create(rsName, RendererDefault, { newPass });

		SHADER_DESC_CORE shaderDesc;
		shaderDesc.setParamBlockAttribs("PerFrame", true, GPBU_DYNAMIC, RBS_PerFrame);
		shaderDesc.setParamBlockAttribs("PerCamera", true, GPBU_DYNAMIC, RBS_PerCamera);
		shaderDesc.setParamBlockAttribs("PerObject", true, GPBU_DYNAMIC, RBS_PerObject);

		shaderDesc.addParameter("gTime", "gTime", GPDT_FLOAT1, RPS_Time);
		shaderDesc.addParameter("gViewDir", "gViewDir", GPDT_FLOAT4, RPS_ViewDir);
		shaderDesc.addParameter("gMatWorldViewProj", "gMatWorldViewProj", GPDT_MATRIX_4X4, RPS_WorldViewProjTfrm);
		shaderDesc.addParameter("gMatViewProj", "gMatViewProj", GPDT_MATRIX_4X4, RPS_ViewProjTfrm);
		shaderDesc.addParameter("gMatWorld", "gMatWorld", GPDT_MATRIX_4X4, RPS_WorldTfrm);
		shaderDesc.addParameter("gMatView", "gMatView", GPDT_MATRIX_4X4, RPS_ViewTfrm);
		shaderDesc.addParameter("gMatProj", "gMatProj", GPDT_MATRIX_4X4, RPS_ProjTfrm);

		SPtr<ShaderCore> defaultShader = ShaderCore::create("LitTexDefault", shaderDesc, { newTechnique });

		return defaultShader;
	}
}