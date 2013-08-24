#include "BsGLBuiltinMaterialFactory.h"
#include "CmHighLevelGpuProgram.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmMaterial.h"
#include "CmBlendState.h"
#include "CmDepthStencilState.h"
#include "CmRendererManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void GLBuiltinMaterialFactory::startUp()
	{
		initSpriteTextShader();
		initSpriteImageShader();
		initDebugDraw2DClipSpaceShader();
		initDebugDraw2DScreenSpaceShader();
		initDebugDraw3DShader();

		SAMPLER_STATE_DESC ssDesc;
		ssDesc.magFilter = FO_POINT;
		ssDesc.minFilter = FO_POINT;
		ssDesc.mipFilter = FO_POINT;

		mGUISamplerState = SamplerState::create(ssDesc);
	}

	void GLBuiltinMaterialFactory::shutDown()
	{
		mSpriteTextShader = nullptr;
		mSpriteImageShader = nullptr;
		mDebugDraw2DClipSpaceShader = nullptr;
		mDebugDraw2DScreenSpaceShader = nullptr;
		mDebugDraw3DShader = nullptr;
	}

	const CM::String& GLBuiltinMaterialFactory::getSupportedRenderSystem() const
	{
		static String renderSystem = "CamelotGLRenderSystem";

		return renderSystem;
	}

	HMaterial GLBuiltinMaterialFactory::createSpriteTextMaterial() const
	{
		HMaterial newMaterial = Material::create(mSpriteTextShader);
		newMaterial->setSamplerState("mainTexSamp", mGUISamplerState);

		return newMaterial;
	}

	HMaterial GLBuiltinMaterialFactory::createSpriteImageMaterial() const
	{
		HMaterial newMaterial = Material::create(mSpriteImageShader);
		newMaterial->setSamplerState("mainTexSamp", mGUISamplerState);

		return newMaterial;
	}

	HMaterial GLBuiltinMaterialFactory::createDebugDraw2DClipSpaceMaterial() const
	{
		return Material::create(mDebugDraw2DClipSpaceShader);
	}

	HMaterial GLBuiltinMaterialFactory::createDebugDraw2DScreenSpaceMaterial() const
	{
		return Material::create(mDebugDraw2DScreenSpaceShader);
	}

	HMaterial GLBuiltinMaterialFactory::createDebugDraw3DMaterial() const
	{
		return Material::create(mDebugDraw3DShader);
	}

	void GLBuiltinMaterialFactory::initSpriteTextShader()
	{
		String vsCode = "#version 400\n							\
						\
						uniform float invViewportWidth;			\
						uniform float invViewportHeight;		\
						uniform mat4 worldTransform;			\
						\
						in vec3 cm_position;					\
						in vec2 cm_texcoord0;					\
						out vec2 texcoord0;						\
						void main()							\
						{																	\
						vec4 tfrmdPos = worldTransform * vec4(cm_position.xy, 0, 1);		\
						\
						float tfrmdX = -1.0f + (tfrmdPos.x * invViewportWidth);				\
						float tfrmdY = 1.0f - (tfrmdPos.y * invViewportHeight);				\
						\
						gl_Position = vec4(tfrmdX, tfrmdY, 0, 1);							\
						texcoord0 = cm_texcoord0;											\
						}																	\
						";

		String psCode = "#version 400\n										\
						\
						uniform sampler2D mainTexture;						\
						in vec2 texcoord0;									\
						out vec4 fragColor;									\
						\
						void main()										\
						{													\
						vec4 color = vec4(1.0f, 1.0f, 1.0f, texture2D(mainTexture, texcoord0.st).r);	\
						fragColor = color;									\
						}";

		HHighLevelGpuProgram vsProgram = HighLevelGpuProgram::create(vsCode, "main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
		HHighLevelGpuProgram psProgram = HighLevelGpuProgram::create(psCode, "main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

		vsProgram.synchronize();
		psProgram.synchronize();

		mSpriteTextShader = Shader::create("TextSpriteShader");

		mSpriteTextShader->addParameter("worldTransform", "worldTransform", GPDT_MATRIX_4X4);
		mSpriteTextShader->addParameter("invViewportWidth", "invViewportWidth", GPDT_FLOAT1);
		mSpriteTextShader->addParameter("invViewportHeight", "invViewportHeight", GPDT_FLOAT1);
		mSpriteTextShader->addParameter("mainTexSamp", "mainTexture", GPOT_SAMPLER2D);
		mSpriteTextShader->addParameter("mainTexture", "mainTexture", GPOT_TEXTURE2D);

		TechniquePtr newTechnique = mSpriteTextShader->addTechnique("GLRenderSystem", RendererManager::getCoreRendererName()); 
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

		DEPTH_STENCIL_STATE_DESC depthStateDesc;
		depthStateDesc.depthReadEnable = false;
		depthStateDesc.depthWriteEnable = false;

		HDepthStencilState depthState = DepthStencilState::create(depthStateDesc);
		newPass->setDepthStencilState(depthState);
	}

	void GLBuiltinMaterialFactory::initSpriteImageShader()
	{
		String vsCode = "#version 400\n							\
						\
						uniform float invViewportWidth;			\
						uniform float invViewportHeight;		\
						uniform mat4 worldTransform;			\
						\
						in vec3 cm_position;					\
						in vec2 cm_texcoord0;					\
						out vec2 texcoord0;						\
						void main()							\
						{																	\
						vec4 tfrmdPos = worldTransform * vec4(cm_position.xy, 0, 1);		\
						\
						float tfrmdX = -1.0f + (tfrmdPos.x * invViewportWidth);				\
						float tfrmdY = 1.0f - (tfrmdPos.y * invViewportHeight);				\
						\
						gl_Position = vec4(tfrmdX, tfrmdY, 0, 1);							\
						texcoord0 = cm_texcoord0;											\
						}																	\
						";

		String psCode = "#version 400\n										\
						\
						uniform sampler2D mainTexture;						\
						in vec2 texcoord0;									\
						out vec4 fragColor;									\
						\
						void main()										\
						{													\
						vec4 color = texture2D(mainTexture, texcoord0.st);	\
						fragColor = color;									\
						}";

		HHighLevelGpuProgram vsProgram = HighLevelGpuProgram::create(vsCode, "main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
		HHighLevelGpuProgram psProgram = HighLevelGpuProgram::create(psCode, "main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

		vsProgram.synchronize();
		psProgram.synchronize();

		mSpriteImageShader = Shader::create("ImageSpriteShader");

		mSpriteImageShader->addParameter("worldTransform", "worldTransform", GPDT_MATRIX_4X4);
		mSpriteImageShader->addParameter("invViewportWidth", "invViewportWidth", GPDT_FLOAT1);
		mSpriteImageShader->addParameter("invViewportHeight", "invViewportHeight", GPDT_FLOAT1);
		mSpriteImageShader->addParameter("mainTexSamp", "mainTexture", GPOT_SAMPLER2D);
		mSpriteImageShader->addParameter("mainTexture", "mainTexture", GPOT_TEXTURE2D);

		TechniquePtr newTechnique = mSpriteImageShader->addTechnique("GLRenderSystem", RendererManager::getCoreRendererName()); 
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

		DEPTH_STENCIL_STATE_DESC depthStateDesc;
		depthStateDesc.depthReadEnable = false;
		depthStateDesc.depthWriteEnable = false;

		HDepthStencilState depthState = DepthStencilState::create(depthStateDesc);
		newPass->setDepthStencilState(depthState);
	}

	void GLBuiltinMaterialFactory::initDebugDraw2DClipSpaceShader()
	{
		String vsCode = "#version 400\n								\
																	\
						in vec2 cm_position;						\
						in vec4 cm_color0;							\
						out vec4 color0;							\
																	\
						void main()									\
						{											\
						gl_Position = vec4(cm_position.xy, 0, 1);	\
						color0 = cm_color0;							\
						}";

		String psCode = "#version 400\n						\
															\
						in vec4 color0;						\
						out vec4 fragColor;					\
															\
						void main()							\
						{									\
						fragColor = color0;					\
						}";

		HHighLevelGpuProgram vsProgram = HighLevelGpuProgram::create(vsCode, "vs_main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
		HHighLevelGpuProgram psProgram = HighLevelGpuProgram::create(psCode, "ps_main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

		vsProgram.synchronize();
		psProgram.synchronize();

		mDebugDraw2DClipSpaceShader = Shader::create("DebugDraw2DClipSpaceShader");

		TechniquePtr newTechnique = mDebugDraw2DClipSpaceShader->addTechnique("GLRenderSystem", RendererManager::getCoreRendererName()); 
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

		DEPTH_STENCIL_STATE_DESC depthStateDesc;
		depthStateDesc.depthReadEnable = false;
		depthStateDesc.depthWriteEnable = false;

		HDepthStencilState depthState = DepthStencilState::create(depthStateDesc);
		newPass->setDepthStencilState(depthState);
	}

	void GLBuiltinMaterialFactory::initDebugDraw2DScreenSpaceShader()
	{
		String vsCode = "#version 400\n								\
																	\
						uniform float invViewportWidth;				\
						uniform float invViewportHeight;			\
																	\
						in vec2 cm_position;						\
						in vec4 cm_color0;							\
						out vec4 color0;							\
																	\
						void main()									\
						{											\
						float tfrmdX = -1.0f + (cm_position.x * invViewportWidth);				\
						float tfrmdY = 1.0f - (cm_position.y * invViewportHeight);				\
																	\
						gl_Position = vec4(tfrmdX, tfrmdY, 0, 1);	\
						color0 = cm_color0;							\
						}";

		String psCode = "#version 400\n						\
															\
						in vec4 color0;						\
						out vec4 fragColor;					\
															\
						void main()							\
						{									\
						fragColor = color0;					\
						}";

		HHighLevelGpuProgram vsProgram = HighLevelGpuProgram::create(vsCode, "vs_main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
		HHighLevelGpuProgram psProgram = HighLevelGpuProgram::create(psCode, "ps_main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

		vsProgram.synchronize();
		psProgram.synchronize();

		mDebugDraw2DScreenSpaceShader = Shader::create("DebugDraw2DScreenSpaceShader");

		TechniquePtr newTechnique = mDebugDraw2DScreenSpaceShader->addTechnique("GLRenderSystem", RendererManager::getCoreRendererName()); 
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

		DEPTH_STENCIL_STATE_DESC depthStateDesc;
		depthStateDesc.depthReadEnable = false;
		depthStateDesc.depthWriteEnable = false;

		HDepthStencilState depthState = DepthStencilState::create(depthStateDesc);
		newPass->setDepthStencilState(depthState);
	}

	void GLBuiltinMaterialFactory::initDebugDraw3DShader()
	{
		String vsCode = "#version 400\n								\
																	\
						uniform mat4 matViewProj;						\
																	\
						in vec3 cm_position;						\
						in vec4 cm_color0;							\
						out vec4 color0;							\
																	\
						void main()									\
						{											\
						gl_Position = matViewProj * vec4(cm_position.xyz, 1);		\
						color0 = cm_color0;							\
						}";

		String psCode = "#version 400\n						\
															\
						in vec4 color0;						\
						out vec4 fragColor;					\
															\
						void main()							\
						{									\
						fragColor = color;					\
						}";

		HHighLevelGpuProgram vsProgram = HighLevelGpuProgram::create(vsCode, "vs_main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
		HHighLevelGpuProgram psProgram = HighLevelGpuProgram::create(psCode, "ps_main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

		vsProgram.synchronize();
		psProgram.synchronize();

		mDebugDraw3DShader = Shader::create("DebugDraw3DShader");

		mDebugDraw3DShader->addParameter("matViewProj", "matViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mDebugDraw3DShader->addTechnique("GLRenderSystem", RendererManager::getCoreRendererName()); 
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

		DEPTH_STENCIL_STATE_DESC depthStateDesc;
		depthStateDesc.depthReadEnable = false;
		depthStateDesc.depthWriteEnable = false;

		HDepthStencilState depthState = DepthStencilState::create(depthStateDesc);
		newPass->setDepthStencilState(depthState);
	}
}