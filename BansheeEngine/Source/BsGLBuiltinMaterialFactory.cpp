#include "BsGLBuiltinMaterialFactory.h"
#include "CmHighLevelGpuProgram.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmMaterial.h"
#include "CmBlendState.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void GLBuiltinMaterialFactory::startUp()
	{
		initSpriteTextShader();
		initSpriteImageShader();
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

	void GLBuiltinMaterialFactory::initSpriteTextShader()
	{
		// TODO
	}

	void GLBuiltinMaterialFactory::initSpriteImageShader()
	{
		// TODO
	}
}