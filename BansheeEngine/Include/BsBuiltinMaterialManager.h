#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMaterialInfo.h"
#include "BsDebugDrawMaterialInfo.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class BS_EXPORT BuiltinMaterialFactory
	{
	public:
		virtual ~BuiltinMaterialFactory() {}

		virtual void startUp() {}
		virtual void shutDown() {}

		virtual const String& getSupportedRenderSystem() const = 0;

		virtual HMaterial createSpriteTextMaterial() const = 0;
		virtual HMaterial createSpriteImageMaterial() const = 0;
		virtual HMaterial createDebugDraw2DClipSpaceMaterial() const = 0;
		virtual HMaterial createDebugDraw2DScreenSpaceMaterial() const = 0;
		virtual HMaterial createDebugDraw3DMaterial() const = 0;
		virtual HMaterial createDockDropOverlayMaterial() const = 0;
	};

	/**
	 * @brief	Provides access to various materials that are required for core engine systems. 
	 * 			Each render system implementation needs to provide its own implementation of this
	 * 			module.
	 */
	class BS_EXPORT BuiltinMaterialManager : public Module<BuiltinMaterialManager>
	{
	public:
		BuiltinMaterialManager();
		~BuiltinMaterialManager();

		GUIMaterialInfo createSpriteTextMaterial() const;
		GUIMaterialInfo createSpriteImageMaterial() const;
		DebugDraw2DClipSpaceMatInfo createDebugDraw2DClipSpaceMaterial() const;
		DebugDraw2DScreenSpaceMatInfo createDebugDraw2DScreenSpaceMaterial() const;
		DebugDraw3DMatInfo createDebugDraw3DMaterial() const;
		HMaterial createDockDropOverlayMaterial() const;

		void addFactory(BuiltinMaterialFactory* factory);
		void setActive(const String& renderSystemName);

	private:
		UnorderedMap<String, BuiltinMaterialFactory*>::type mAvailableFactories;
		BuiltinMaterialFactory* mActiveFactory;
	};
}