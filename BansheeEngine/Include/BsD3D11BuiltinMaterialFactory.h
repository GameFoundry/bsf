#pragma once

#include "BsPrerequisites.h"
#include "BsBuiltinMaterialManager.h"

namespace BansheeEngine
{
	/**
	 * @copydoc BuiltinMaterialFactory
	 */
	class D3D11BuiltinMaterialFactory : public BuiltinMaterialFactory
	{
	public:
		void startUp();
		void shutDown();
		const CM::String& getSupportedRenderSystem() const;
		
		CM::HMaterial createSpriteTextMaterial() const;
		CM::HMaterial createSpriteImageMaterial() const;

	protected:
		CM::ShaderPtr mSpriteTextShader;
		CM::ShaderPtr mSpriteImageShader;

		void initSpriteTextShader();
		void initSpriteImageShader();
	};
}