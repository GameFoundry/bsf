#include "BsRendererMaterialManager.h"
#include "BsBuiltinResources.h"
#include "BsCoreThread.h"
#include "BsShader.h"

namespace BansheeEngine
{
	Vector<RendererMaterialBase*> RendererMaterialManager::mMaterialsToInit;
	Mutex RendererMaterialManager::mMutex;

	RendererMaterialManager::RendererMaterialManager()
	{
		BuiltinResources& br = BuiltinResources::instance();

		Vector<SPtr<ShaderCore>> shaders;
		for (auto& material : mMaterialsToInit)
		{
			HShader shader = br.getShader(material->getShaderPath());
			if (shader.isLoaded())
				shaders.push_back(shader->getCore());
			else
				shaders.push_back(nullptr);
		}

		gCoreAccessor().queueCommand(std::bind(&RendererMaterialManager::initOnCore, shaders));
	}

	void RendererMaterialManager::_registerMaterial(RendererMaterialBase* data)
	{
		Lock<> lock(mMutex);

		mMaterialsToInit.push_back(data);
	}

	void RendererMaterialManager::initOnCore(const Vector<SPtr<ShaderCore>>& shaders)
	{
		Lock<> lock(mMutex);

		for (UINT32 i = 0; i < mMaterialsToInit.size(); i++)
		{
			mMaterialsToInit[i]->_initialize(shaders[i]);
		}
	}
}