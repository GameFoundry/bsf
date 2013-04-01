#include "CmGLBuiltinMaterialManager.h"
#include "CmHighLevelGpuProgram.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmMaterial.h"
#include "CmBlendState.h"

namespace CamelotEngine
{
	MaterialHandle GLBuiltinMaterialManager::createSpriteTextMaterial() const
	{
		return Material::create(mSpriteTextShader);
	}

	MaterialHandle GLBuiltinMaterialManager::createSpriteImageMaterial() const
	{
		return Material::create(mSpriteImageShader);
	}

	void GLBuiltinMaterialManager::onStartUp()
	{
		initSpriteTextShader();
		initSpriteImageShader();
	}

	void GLBuiltinMaterialManager::initSpriteTextShader()
	{
		// TODO
	}

	void GLBuiltinMaterialManager::initSpriteImageShader()
	{
		// TODO
	}
}