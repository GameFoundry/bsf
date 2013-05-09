#include "BsGLBuiltinMaterialFactory.h"
#include "CmHighLevelGpuProgram.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmMaterial.h"
#include "CmBlendState.h"
#include "CmRendererManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void GLBuiltinMaterialFactory::startUp()
	{
		initSpriteTextShader();
		initSpriteImageShader();
		initDebugDrawShader();
	}

	void GLBuiltinMaterialFactory::shutDown()
	{
		mSpriteTextShader = nullptr;
		mSpriteImageShader = nullptr;
	}

	const CM::String& GLBuiltinMaterialFactory::getSupportedRenderSystem() const
	{
		static String renderSystem = "CamelotGLRenderSystem";

		return renderSystem;
	}

	HMaterial GLBuiltinMaterialFactory::createSpriteTextMaterial() const
	{
		return Material::create(mSpriteTextShader);
	}

	HMaterial GLBuiltinMaterialFactory::createSpriteImageMaterial() const
	{
		return Material::create(mSpriteImageShader);
	}

	HMaterial GLBuiltinMaterialFactory::createDebugDrawMaterial() const
	{
		return Material::create(mDebugDrawShader);
	}

	void GLBuiltinMaterialFactory::initSpriteTextShader()
	{
		String vsCode = "#version 400\n							\
						\
						uniform float halfViewportWidth;		\
						uniform float halfViewportHeight;		\
						uniform mat4 worldTransform;			\
						\
						in vec3 cm_position;					\
						in vec2 cm_texcoord0;					\
						out vec2 texcoord0;						\
						void main()							\
						{																	\
						vec4 tfrmdPos = worldTransform * vec4(cm_position.xy, 0, 1);		\
						\
						float tfrmdX = (tfrmdPos.x / halfViewportWidth) - 1.0f;	\
						float tfrmdY = (tfrmdPos.y / halfViewportHeight) + 1.0f;	\
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

		vsProgram.waitUntilLoaded();
		psProgram.waitUntilLoaded();

		mSpriteTextShader = Shader::create("TextSpriteShader");

		mSpriteTextShader->addParameter("worldTransform", "worldTransform", GPDT_MATRIX_4X4);
		mSpriteTextShader->addParameter("halfViewportWidth", "halfViewportWidth", GPDT_FLOAT1);
		mSpriteTextShader->addParameter("halfViewportHeight", "halfViewportHeight", GPDT_FLOAT1);
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
	}

	void GLBuiltinMaterialFactory::initSpriteImageShader()
	{
		String vsCode = "#version 400\n							\
						\
						uniform float halfViewportWidth;		\
						uniform float halfViewportHeight;		\
						uniform mat4 worldTransform;			\
						\
						in vec3 cm_position;					\
						in vec2 cm_texcoord0;					\
						out vec2 texcoord0;						\
						void main()							\
						{																	\
						vec4 tfrmdPos = worldTransform * vec4(cm_position.xy, 0, 1);		\
						\
						float tfrmdX = (tfrmdPos.x / halfViewportWidth) - 1.0f;	\
						float tfrmdY = (tfrmdPos.y / halfViewportHeight) + 1.0f;	\
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

		vsProgram.waitUntilLoaded();
		psProgram.waitUntilLoaded();

		mSpriteImageShader = Shader::create("ImageSpriteShader");

		mSpriteImageShader->addParameter("worldTransform", "worldTransform", GPDT_MATRIX_4X4);
		mSpriteImageShader->addParameter("halfViewportWidth", "halfViewportWidth", GPDT_FLOAT1);
		mSpriteImageShader->addParameter("halfViewportHeight", "halfViewportHeight", GPDT_FLOAT1);
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
	}

	void GLBuiltinMaterialFactory::initDebugDrawShader()
	{
		// TODO - Not implemented
	}
}