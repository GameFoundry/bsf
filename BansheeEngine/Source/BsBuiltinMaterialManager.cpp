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
			cm_delete(iter.second);
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

	HMaterial BuiltinMaterialManager::createDebugDraw2DClipSpaceMaterial() const
	{
		assert(mActiveFactory != nullptr);

		return mActiveFactory->createDebugDraw2DClipSpaceMaterial();
	}

	HMaterial BuiltinMaterialManager::createDebugDraw2DScreenSpaceMaterial() const
	{
		assert(mActiveFactory != nullptr);

		return mActiveFactory->createDebugDraw2DScreenSpaceMaterial();
	}

	HMaterial BuiltinMaterialManager::createDebugDraw3DMaterial() const
	{
		assert(mActiveFactory != nullptr);

		return mActiveFactory->createDebugDraw3DMaterial();
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