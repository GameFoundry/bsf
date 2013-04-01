#include "CmD3D11BuiltinMaterialManager.h"
#include "CmRendererManager.h"
#include "CmHighLevelGpuProgram.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmMaterial.h"
#include "CmBlendState.h"

namespace CamelotEngine
{
	MaterialHandle D3D11BuiltinMaterialManager::createSpriteTextMaterial() const
	{
		return Material::create(mSpriteTextShader);
	}

	MaterialHandle D3D11BuiltinMaterialManager::createSpriteImageMaterial() const
	{
		return Material::create(mSpriteImageShader);
	}

	void D3D11BuiltinMaterialManager::onStartUp()
	{
		initSpriteTextShader();
		initSpriteImageShader();
	}

	void D3D11BuiltinMaterialManager::initSpriteTextShader()
	{
		String vsCode = "										\
			void vs_main(										\
			in float3 inPos : POSITION,							\
			in float2 uv : TEXCOORD0,							\
			out float4 oPosition : SV_Position,					\
			out float2 oUv : TEXCOORD0)							\
			{													\
				float tfrmdX = (inPos.x / 640) - 1.0f;			\
				float tfrmdY = (inPos.y / 360) + 1.0f;			\
																\
				oPosition = float4(tfrmdX, tfrmdY, 0, 1);		\
				oUv = uv;										\
			}													\
			";

		String psCode = "																			\
			SamplerState mainTexSamp : register(s0);												\
			Texture2D mainTexture : register(t0);													\
																									\
			float4 ps_main(in float4 inPos : SV_Position, float2 uv : TEXCOORD0) : SV_Target		\
			{																						\
				float4 color = float4(1.0f, 1.0f, 1.0f, mainTexture.Sample(mainTexSamp, uv).r);		\
				return color;																		\
			}";

		HighLevelGpuProgramHandle vsProgram = HighLevelGpuProgram::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
		HighLevelGpuProgramHandle psProgram = HighLevelGpuProgram::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

		vsProgram.waitUntilLoaded();
		psProgram.waitUntilLoaded();

		mSpriteTextShader = Shader::create("TextShader");

		mSpriteTextShader->addParameter("mainTexSamp", "mainTexSamp", GPOT_SAMPLER2D);
		mSpriteTextShader->addParameter("mainTexture", "mainTexture", GPOT_TEXTURE2D);

		TechniquePtr newTechnique = mSpriteTextShader->addTechnique("D3D11RenderSystem", RendererManager::getCoreRendererName()); 
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		BLEND_STATE_DESC desc;
		desc.renderTargetDesc[0].blendEnable = true;
		desc.renderTargetDesc[0].srcBlend = BF_SOURCE_ALPHA;
		desc.renderTargetDesc[0].dstBlend = BF_INV_SOURCE_ALPHA;
		desc.renderTargetDesc[0].blendOp = BO_ADD;

		BlendStateHandle blendState = BlendState::create(desc);
		newPass->setBlendState(blendState);
	}

	void D3D11BuiltinMaterialManager::initSpriteImageShader()
	{
		// TODO
	}
}