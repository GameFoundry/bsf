#pragma once

#include "BsPrerequisites.h"
#include "BsBuiltinMaterialManager.h"

namespace BansheeEngine
{
	/**
	 * @copydoc BuiltinMaterialFactory
	 */
	class GLBuiltinMaterialFactory : public BuiltinMaterialFactory
	{
	public:
		void startUp();
		void shutDown();
		const CM::String& getSupportedRenderSystem() const;

		CM::HMaterial createSpriteTextMaterial() const;
		CM::HMaterial createSpriteImageMaterial() const;
		CM::HMaterial createDebugDrawMaterial() const;

	protected:
		CM::ShaderPtr mSpriteTextShader;
		CM::ShaderPtr mSpriteImageShader;
		CM::ShaderPtr mDebugDrawShader;

		CM::HSamplerState mGUISamplerState;

		void initSpriteTextShader();
		void initSpriteImageShader();
		void initDebugDrawShader();
	};
}