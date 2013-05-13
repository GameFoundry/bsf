#include "BsD3D11BuiltinMaterialFactory.h"
#include "CmRendererManager.h"
#include "CmHighLevelGpuProgram.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmMaterial.h"
#include "CmBlendState.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void D3D11BuiltinMaterialFactory::startUp()
	{
		initSpriteTextShader();
		initSpriteImageShader();
		initDebugDrawShader();

		SAMPLER_STATE_DESC ssDesc;
		ssDesc.magFilter = FO_POINT;
		ssDesc.minFilter = FO_POINT;
		ssDesc.mipFilter = FO_POINT;

		mGUISamplerState = SamplerState::create(ssDesc);
	}

	void D3D11BuiltinMaterialFactory::shutDown()
	{
		mSpriteTextShader = nullptr;
		mSpriteImageShader = nullptr;
	}

	const CM::String& D3D11BuiltinMaterialFactory::getSupportedRenderSystem() const
	{
		static String renderSystem = "CamelotD3D11RenderSystem";

		return renderSystem;
	}

	HMaterial D3D11BuiltinMaterialFactory::createSpriteTextMaterial() const
	{
		HMaterial newMaterial = Material::create(mSpriteTextShader);
		newMaterial->setSamplerState("mainTexSamp", mGUISamplerState);

		return newMaterial;
	}

	HMaterial D3D11BuiltinMaterialFactory::createSpriteImageMaterial() const
	{
		HMaterial newMaterial = Material::create(mSpriteImageShader);
		newMaterial->setSamplerState("mainTexSamp", mGUISamplerState);

		return newMaterial;
	}

	HMaterial D3D11BuiltinMaterialFactory::createDebugDrawMaterial() const
	{
		return Material::create(mDebugDrawShader);
	}

	void D3D11BuiltinMaterialFactory::initSpriteTextShader()
	{
		String vsCode = "										\
			float invViewportWidth;								\
			float invViewportHeight;							\
			float4x4 worldTransform;							\
																\
			void vs_main(										\
			in float3 inPos : POSITION,							\
			in float2 uv : TEXCOORD0,							\
			out float4 oPosition : SV_Position,					\
			out float2 oUv : TEXCOORD0)							\
			{													\
				float4 tfrmdPos = mul(worldTransform, float4(inPos.xy, 0, 1));	\
																				\
				float tfrmdX = -1.0f + (tfrmdPos.x * invViewportWidth);			\
				float tfrmdY = 1.0f - (tfrmdPos.y * invViewportHeight);			\
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

		HHighLevelGpuProgram vsProgram = HighLevelGpuProgram::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
		HHighLevelGpuProgram psProgram = HighLevelGpuProgram::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

		vsProgram.waitUntilLoaded();
		psProgram.waitUntilLoaded();

		mSpriteTextShader = Shader::create("TextSpriteShader");

		mSpriteTextShader->addParameter("worldTransform", "worldTransform", GPDT_MATRIX_4X4);
		mSpriteTextShader->addParameter("invViewportWidth", "invViewportWidth", GPDT_FLOAT1);
		mSpriteTextShader->addParameter("invViewportHeight", "invViewportHeight", GPDT_FLOAT1);
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

		HBlendState blendState = BlendState::create(desc);
		newPass->setBlendState(blendState);
	}

	void D3D11BuiltinMaterialFactory::initSpriteImageShader()
	{
		String vsCode = "										\
						float invViewportWidth;								\
						float invViewportHeight;							\
						float4x4 worldTransform;							\
						\
						void vs_main(										\
						in float3 inPos : POSITION,							\
						in float2 uv : TEXCOORD0,							\
						out float4 oPosition : SV_Position,					\
						out float2 oUv : TEXCOORD0)							\
						{													\
						float4 tfrmdPos = mul(worldTransform, float4(inPos.xy, 0, 1));	\
						\
						float tfrmdX = -1.0f + (tfrmdPos.x * invViewportWidth);			\
						float tfrmdY = 1.0f - (tfrmdPos.y * invViewportHeight);			\
						\
						oPosition = float4(tfrmdX, tfrmdY, 0, 1);		\
						oUv = uv;										\
						}													\
						";

		String psCode = "																					\
						SamplerState mainTexSamp : register(s0);											\
						Texture2D mainTexture : register(t0);												\
																											\
						float4 ps_main(in float4 inPos : SV_Position, float2 uv : TEXCOORD0) : SV_Target	\
						{																					\
						float4 color = mainTexture.Sample(mainTexSamp, uv);									\
						return color;																		\
						}";

		HHighLevelGpuProgram vsProgram = HighLevelGpuProgram::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
		HHighLevelGpuProgram psProgram = HighLevelGpuProgram::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

		vsProgram.waitUntilLoaded();
		psProgram.waitUntilLoaded();

		mSpriteImageShader = Shader::create("ImageSpriteShader");

		mSpriteImageShader->addParameter("worldTransform", "worldTransform", GPDT_MATRIX_4X4);
		mSpriteImageShader->addParameter("invViewportWidth", "invViewportWidth", GPDT_FLOAT1);
		mSpriteImageShader->addParameter("invViewportHeight", "invViewportHeight", GPDT_FLOAT1);
		mSpriteImageShader->addParameter("mainTexSamp", "mainTexSamp", GPOT_SAMPLER2D);
		mSpriteImageShader->addParameter("mainTexture", "mainTexture", GPOT_TEXTURE2D);

		TechniquePtr newTechnique = mSpriteImageShader->addTechnique("D3D11RenderSystem", RendererManager::getCoreRendererName()); 
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		BLEND_STATE_DESC desc;
		desc.renderTargetDesc[0].blendEnable = true;
		desc.renderTargetDesc[0].srcBlend = BF_SOURCE_ALPHA;
		desc.renderTargetDesc[0].dstBlend = BF_INV_SOURCE_ALPHA;
		desc.renderTargetDesc[0].blendOp = BO_ADD;

		HBlendState blendState = BlendState::create(desc);
		newPass->setBlendState(blendState);
	}

	void D3D11BuiltinMaterialFactory::initDebugDrawShader()
	{
		String vsCode = "						\
		float4x4 matViewProjection;				\
												\
		void vs_main(							\
			in float4 inPos : POSITION,			\
			in float4 color : COLOR,			\
			out float4 oPosition : SV_Position, \
			out float4 oColor : COLOR)			\
		{										\
			oPosition = mul(matViewProjection, inPos); \
			oColor = color;						\
		}";

		String psCode = "																		\
		float4 ps_main(in float4 inPos : SV_Position, in float4 color : COLOR) : SV_Target		\
		{																						\
			return color;																		\
		}																						\
		";	

		HHighLevelGpuProgram vsProgram = HighLevelGpuProgram::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
		HHighLevelGpuProgram psProgram = HighLevelGpuProgram::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

		vsProgram.waitUntilLoaded();
		psProgram.waitUntilLoaded();

		mDebugDrawShader = Shader::create("DebugDrawShader");

		mDebugDrawShader->addParameter("matViewProjection", "matViewProjection", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mDebugDrawShader->addTechnique("D3D11RenderSystem", RendererManager::getCoreRendererName()); 
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);
	}
}