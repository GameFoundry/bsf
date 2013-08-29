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
		CM::HMaterial createDebugDraw2DClipSpaceMaterial() const;
		CM::HMaterial createDebugDraw2DScreenSpaceMaterial() const;
		CM::HMaterial createDebugDraw3DMaterial() const;
		CM::HMaterial createDockDropOverlayMaterial() const;

	protected:
		CM::ShaderPtr mSpriteTextShader;
		CM::ShaderPtr mSpriteImageShader;
		CM::ShaderPtr mDebugDraw2DClipSpaceShader;
		CM::ShaderPtr mDebugDraw2DScreenSpaceShader;
		CM::ShaderPtr mDebugDraw3DShader;
		CM::ShaderPtr mDockDropOverlayShader;

		CM::HSamplerState mGUISamplerState;

		void initSpriteTextShader();
		void initSpriteImageShader();
		void initDebugDraw2DClipSpaceShader();
		void initDebugDraw2DScreenSpaceShader();
		void initDebugDraw3DShader();
		void initDockDropOverlayShader();
	};
}