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
		const String& getSupportedRenderSystem() const;

		HMaterial createSpriteTextMaterial() const;
		HMaterial createSpriteImageMaterial() const;
		HMaterial createDebugDraw2DClipSpaceMaterial() const;
		HMaterial createDebugDraw2DScreenSpaceMaterial() const;
		HMaterial createDebugDraw3DMaterial() const;
		HMaterial createDockDropOverlayMaterial() const;
		HMaterial createDummyMaterial() const;

	protected:
		ShaderPtr mSpriteTextShader;
		ShaderPtr mSpriteImageShader;
		ShaderPtr mDebugDraw2DClipSpaceShader;
		ShaderPtr mDebugDraw2DScreenSpaceShader;
		ShaderPtr mDebugDraw3DShader;
		ShaderPtr mDockDropOverlayShader;
		ShaderPtr mDummyShader;

		HSamplerState mGUISamplerState;

		void initSpriteTextShader();
		void initSpriteImageShader();
		void initDebugDraw2DClipSpaceShader();
		void initDebugDraw2DScreenSpaceShader();
		void initDebugDraw3DShader();
		void initDockDropOverlayShader();
		void initDummyShader();
	};
}