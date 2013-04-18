#include "BsD3D9BuiltinMaterialFactory.h"
#include "CmHighLevelGpuProgram.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmMaterial.h"
#include "CmBlendState.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void D3D9BuiltinMaterialFactory::startUp()
	{
		initSpriteTextShader();
		initSpriteImageShader();
	}

	void D3D9BuiltinMaterialFactory::shutDown()
	{
		mSpriteTextShader = nullptr;
		mSpriteImageShader = nullptr;
	}

	const CM::String& D3D9BuiltinMaterialFactory::getSupportedRenderSystem() const
	{
		static String renderSystem = "CamelotD3D9RenderSystem";

		return renderSystem;
	}

	HMaterial D3D9BuiltinMaterialFactory::createSpriteTextMaterial() const
	{
		return Material::create(mSpriteTextShader);
	}

	HMaterial D3D9BuiltinMaterialFactory::createSpriteImageMaterial() const
	{
		return Material::create(mSpriteImageShader);
	}


	void D3D9BuiltinMaterialFactory::initSpriteTextShader()
	{
		// TODO
	}

	void D3D9BuiltinMaterialFactory::initSpriteImageShader()
	{
		// TODO
	}
}