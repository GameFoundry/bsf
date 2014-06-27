#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMaterialInfo.h"
#include "BsDebugDrawMaterialInfo.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of built-in materials for a specific render system.
	 *			Needs to be specialized for each used render system.
	 */
	class BS_EXPORT BuiltinMaterialFactory
	{
	public:
		virtual ~BuiltinMaterialFactory() {}

		/**
		 * @brief	Triggered when the factory is made active.
		 */
		virtual void startUp() {}

		/**
		 * @brief	Triggered before the factory is made inactive.
		 */
		virtual void shutDown() {}

		/**
		 * @brief	Returns the name of the render system this factory creates materials for.
		 */
		virtual const String& getSupportedRenderSystem() const = 0;

		/**
		 * @brief	Creates material used for textual sprite rendering (e.g. text in GUI).
		 */
		virtual HMaterial createSpriteTextMaterial() const = 0;

		/**
		 * @brief	Creates material used for image sprite rendering (e.g. images in GUI).
		 */
		virtual HMaterial createSpriteImageMaterial() const = 0;

		/**
		 * @brief	Creates a material used for debug drawing in clip space (e.g. 2D shapes that resize with the viewport).
		 */
		virtual HMaterial createDebugDraw2DClipSpaceMaterial() const = 0;

		/**
		 * @brief	Creates a material used for debug drawing in screen space (e.g. 2D shapes of fixed pixel size).
		 */
		virtual HMaterial createDebugDraw2DScreenSpaceMaterial() const = 0;

		/**
		 * @brief	Creates a material used for debug drawing in 3D.
		 */
		virtual HMaterial createDebugDraw3DMaterial() const = 0;

		/**
		 * @brief	Creates a material used for docking drop overlay used by the editor.
		 *
		 * TODO: Move this to Editor as it's editor-specific.
		 */
		virtual HMaterial createDockDropOverlayMaterial() const = 0;

		/**
		 * @brief	Creates a material used as a replacement when no other material is usable.
		 */
		virtual HMaterial createDummyMaterial() const = 0;
	};

	/**
	 * @brief	Provides access to various materials that are required for core engine systems. 
	 * 			Each render system implementation needs to provide its own implementation of
	 *			BuiltinMaterialFactory and register it with this module.
	 */
	class BS_EXPORT BuiltinMaterialManager : public Module<BuiltinMaterialManager>
	{
	public:
		BuiltinMaterialManager();
		~BuiltinMaterialManager();

		/**
		 * @copydoc	BuiltinMaterialFactory::createSpriteTextMaterial
		 */
		GUIMaterialInfo createSpriteTextMaterial() const;

		/**
		 * @copydoc	BuiltinMaterialFactory::createSpriteImageMaterial
		 */
		GUIMaterialInfo createSpriteImageMaterial() const;

		/**
		 * @copydoc	BuiltinMaterialFactory::createDebugDraw2DClipSpaceMaterial
		 */
		DebugDraw2DClipSpaceMatInfo createDebugDraw2DClipSpaceMaterial() const;

		/**
		 * @copydoc	BuiltinMaterialFactory::createDebugDraw2DScreenSpaceMaterial
		 */
		DebugDraw2DScreenSpaceMatInfo createDebugDraw2DScreenSpaceMaterial() const;

		/**
		 * @copydoc	BuiltinMaterialFactory::createDebugDraw3DMaterial
		 */
		DebugDraw3DMatInfo createDebugDraw3DMaterial() const;

		/**
		 * @copydoc	BuiltinMaterialFactory::createDockDropOverlayMaterial
		 */
		HMaterial createDockDropOverlayMaterial() const; // TODO - This belongs in editor

		/**
		 * @copydoc	BuiltinMaterialFactory::createDummyMaterial
		 */
		HMaterial createDummyMaterial() const;

		/**
		 * @brief	Registers a new material creation factory.
		 */
		void addFactory(BuiltinMaterialFactory* factory);

		/**
		 * @brief	Attempts to find a factory responsible for creating materials for the provided
		 *			render system and makes it active.
		 */
		void setActive(const String& renderSystemName);

	private:
		UnorderedMap<String, BuiltinMaterialFactory*> mAvailableFactories;
		BuiltinMaterialFactory* mActiveFactory;
	};
}