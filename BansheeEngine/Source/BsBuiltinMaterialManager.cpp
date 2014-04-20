#include "BsBuiltinMaterialManager.h"
#include "CmMaterial.h"

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

	GUIMaterialInfo BuiltinMaterialManager::createSpriteTextMaterial() const
	{
		assert(mActiveFactory != nullptr);

		GUIMaterialInfo info;
		info.material = mActiveFactory->createSpriteTextMaterial();
		info.invViewportWidth = info.material->getParamFloat("invViewportWidth");
		info.invViewportHeight = info.material->getParamFloat("invViewportHeight");
		info.worldTransform = info.material->getParamMat4("worldTransform");
		info.mainTexture = info.material->getParamTexture("mainTexture");
		info.tint = info.material->getParamVec4("tint");

		return info;
	}

	GUIMaterialInfo BuiltinMaterialManager::createSpriteImageMaterial() const
	{
		assert(mActiveFactory != nullptr);

		GUIMaterialInfo info;
		info.material = mActiveFactory->createSpriteImageMaterial();
		info.invViewportWidth = info.material->getParamFloat("invViewportWidth");
		info.invViewportHeight = info.material->getParamFloat("invViewportHeight");
		info.worldTransform = info.material->getParamMat4("worldTransform");
		info.mainTexture = info.material->getParamTexture("mainTexture");
		info.tint = info.material->getParamVec4("tint");

		return info;
	}

	DebugDraw2DClipSpaceMatInfo BuiltinMaterialManager::createDebugDraw2DClipSpaceMaterial() const
	{
		assert(mActiveFactory != nullptr);

		DebugDraw2DClipSpaceMatInfo info;
		info.material = mActiveFactory->createDebugDraw2DClipSpaceMaterial();

		return info;
	}

	DebugDraw2DScreenSpaceMatInfo BuiltinMaterialManager::createDebugDraw2DScreenSpaceMaterial() const
	{
		assert(mActiveFactory != nullptr);

		DebugDraw2DScreenSpaceMatInfo info;
		info.material = mActiveFactory->createDebugDraw2DScreenSpaceMaterial();
		info.invViewportWidth = info.material->getParamFloat("invViewportWidth");
		info.invViewportHeight = info.material->getParamFloat("invViewportHeight");

		return info;
	}

	DebugDraw3DMatInfo BuiltinMaterialManager::createDebugDraw3DMaterial() const
	{
		assert(mActiveFactory != nullptr);

		DebugDraw3DMatInfo info;
		info.material = mActiveFactory->createDebugDraw3DMaterial();
		info.matViewProj = info.material->getParamMat4("matViewProj");

		return info;
	}

	HMaterial BuiltinMaterialManager::createDockDropOverlayMaterial() const
	{
		assert(mActiveFactory != nullptr);

		return mActiveFactory->createDockDropOverlayMaterial();
	}

	void BuiltinMaterialManager::addFactory(BuiltinMaterialFactory* factory)
	{
		assert(factory != nullptr);

		mAvailableFactories[factory->getSupportedRenderSystem()] = factory;
	}

	void BuiltinMaterialManager::setActive(const String& renderSystemName)
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