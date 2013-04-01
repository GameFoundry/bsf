#include "CmD3D9BuiltinMaterialManager.h"
#include "CmHighLevelGpuProgram.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmMaterial.h"
#include "CmBlendState.h"

namespace CamelotEngine
{
	MaterialHandle D3D9BuiltinMaterialManager::createSpriteTextMaterial() const
	{
		return Material::create(mSpriteTextShader);
	}

	MaterialHandle D3D9BuiltinMaterialManager::createSpriteImageMaterial() const
	{
		return Material::create(mSpriteImageShader);
	}

	void D3D9BuiltinMaterialManager::onStartUp()
	{
		initSpriteTextShader();
		initSpriteImageShader();
	}

	void D3D9BuiltinMaterialManager::initSpriteTextShader()
	{
		// TODO
	}

	void D3D9BuiltinMaterialManager::initSpriteImageShader()
	{
		// TODO
	}
}