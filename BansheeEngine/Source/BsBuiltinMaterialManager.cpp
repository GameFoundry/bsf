#include "BsBuiltinMaterialManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	BuiltinMaterialManager::BuiltinMaterialManager()
		:mActiveFactory(nullptr)
	{

	}

	BuiltinMaterialManager::~BuiltinMaterialManager()
	{
		for(auto& iter : mAvailableFactories)
		{
			iter.second->shutDown();
			CM_DELETE(iter.second, BuiltinMaterialFactory, GenAlloc)
		}
	}

	HMaterial BuiltinMaterialManager::createSpriteTextMaterial() const
	{
		assert(mActiveFactory != nullptr);

		return mActiveFactory->createSpriteTextMaterial();
	}

	HMaterial BuiltinMaterialManager::createSpriteImageMaterial() const
	{
		assert(mActiveFactory != nullptr);

		return mActiveFactory->createSpriteImageMaterial();
	}

	HMaterial BuiltinMaterialManager::createDebugDrawMaterial() const
	{
		assert(mActiveFactory != nullptr);

		return mActiveFactory->createDebugDrawMaterial();
	}

	void BuiltinMaterialManager::addFactory(BuiltinMaterialFactory* factory)
	{
		assert(factory != nullptr);

		mAvailableFactories[factory->getSupportedRenderSystem()] = factory;
	}

	void BuiltinMaterialManager::setActive(const CM::String& renderSystemName)
	{
		auto iterFind = mAvailableFactories.find(renderSystemName);

		if(iterFind == mAvailableFactories.end())
		{
			CM_EXCEPT(InvalidParametersException, "Cannot find a factory for the specified render system: " + renderSystemName);
		}

		mActiveFactory = iterFind->second;
		mActiveFactory->startUp();
	}
}