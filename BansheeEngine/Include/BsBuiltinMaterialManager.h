#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class BS_EXPORT BuiltinMaterialFactory
	{
	public:
		virtual ~BuiltinMaterialFactory() {}

		virtual void startUp() {}
		virtual void shutDown() {}

		virtual const CM::String& getSupportedRenderSystem() const = 0;

		virtual CM::HMaterial createSpriteTextMaterial() const = 0;
		virtual CM::HMaterial createSpriteImageMaterial() const = 0;
	};

	/**
	 * @brief	Provides access to various materials that are required for core engine systems. 
	 * 			Each render system implementation needs to provide its own implementation of this
	 * 			module.
	 */
	class BS_EXPORT BuiltinMaterialManager : public CM::Module<BuiltinMaterialManager>
	{
	public:
		BuiltinMaterialManager();
		~BuiltinMaterialManager();

		CM::HMaterial createSpriteTextMaterial() const;
		CM::HMaterial createSpriteImageMaterial() const;

		void addFactory(BuiltinMaterialFactory* factory);
		void setActive(const CM::String& renderSystemName);

	private:
		std::unordered_map<CM::String, BuiltinMaterialFactory*> mAvailableFactories;
		BuiltinMaterialFactory* mActiveFactory;
	};
}