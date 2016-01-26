#pragma once

#include "BsPrerequisites.h"
#include "BsBuiltinMaterialManager.h"

namespace BansheeEngine
{
	/**
	 * @copydoc BuiltinMaterialFactory
	 */
	class D3D9BuiltinMaterialFactory : public BuiltinMaterialFactory
	{
	public:
		/** @copydoc BuiltinMaterialFactory::startUp */
		void startUp();

		/** @copydoc BuiltinMaterialFactory::startUp */
		void shutDown();

		/** @copydoc BuiltinMaterialFactory::getSupportedRenderSystem */
		const String& getSupportedRenderSystem() const;

		/** @copydoc BuiltinMaterialFactory::createSpriteTextMaterial */
		HMaterial createSpriteTextMaterial() const;

		/** @copydoc BuiltinMaterialFactory::createSpriteImageMaterial */
		HMaterial createSpriteImageMaterial() const;

		/** @copydoc BuiltinMaterialFactory::createDebugDraw2DClipSpaceMaterial */
		HMaterial createDebugDraw2DClipSpaceMaterial() const;

		/** @copydoc BuiltinMaterialFactory::createDebugDraw2DScreenSpaceMaterial */
		HMaterial createDebugDraw2DScreenSpaceMaterial() const;

		/** @copydoc BuiltinMaterialFactory::createDebugDraw3DMaterial */
		HMaterial createDebugDraw3DMaterial() const;

		/** @copydoc BuiltinMaterialFactory::createDockDropOverlayMaterial */
		HMaterial createDockDropOverlayMaterial() const;

		/** @copydoc BuiltinMaterialFactory::createDummyMaterial */
		HMaterial createDummyMaterial() const;

	protected:
		/**
		 * @brief	Loads an compiles a shader for text rendering.
		 */
		void initSpriteTextShader();

		/**
		 * @brief	Loads an compiles a shader for image sprite rendering.
		 */
		void initSpriteImageShader();

		/**
		 * @brief	Loads an compiles a shader for debug 2D clip space rendering.
		 */
		void initDebugDraw2DClipSpaceShader();

		/**
		 * @brief	Loads an compiles a shader for debug 2D screen space rendering.
		 */
		void initDebugDraw2DScreenSpaceShader();

		/**
		 * @brief	Loads an compiles a shader for debug 3D rendering.
		 */
		void initDebugDraw3DShader();

		/**
		 * @brief	Loads an compiles a shader for dock overlay rendering.
		 */
		void initDockDropOverlayShader();

		/**
		 * @brief	Loads an compiles a simple shader to be used with no other is usable.
		 */
		void initDummyShader();

		ShaderPtr mSpriteTextShader;
		ShaderPtr mSpriteImageShader;
		ShaderPtr mDebugDraw2DClipSpaceShader;
		ShaderPtr mDebugDraw2DScreenSpaceShader;
		ShaderPtr mDebugDraw3DShader;
		ShaderPtr mDockDropOverlayShader;
		ShaderPtr mDummyShader;

		HSamplerState mGUISamplerState;
	};
}