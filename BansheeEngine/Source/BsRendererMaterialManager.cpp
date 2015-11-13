#include "BsRendererMaterialManager.h"
#include "BsBuiltinResources.h"
#include "BsCoreThread.h"
#include "BsShader.h"

namespace BansheeEngine
{
	RendererMaterialManager::RendererMaterialManager()
	{
		BuiltinResources& br = BuiltinResources::instance();

		Vector<RendererMaterialData>& materials = getMaterials();
		Vector<SPtr<ShaderCore>> shaders;
		for (auto& material : materials)
		{
			HShader shader = br.getShader(material.shaderPath);
			if (shader.isLoaded())
				shaders.push_back(shader->getCore());
			else
				shaders.push_back(nullptr);
		}

		gCoreThread().queueCommand(std::bind(&RendererMaterialManager::initOnCore, shaders));
	}

	RendererMaterialManager::~RendererMaterialManager()
	{
		gCoreAccessor().queueCommand(std::bind(&RendererMaterialManager::destroyOnCore));
	}

	void RendererMaterialManager::_registerMaterial(RendererMaterialMetaData* metaData, const Path& shaderPath)
	{
		Lock<> lock(getMutex());

		Vector<RendererMaterialData>& materials = getMaterials();
		materials.push_back({metaData, shaderPath});
	}

	void RendererMaterialManager::initOnCore(const Vector<SPtr<ShaderCore>>& shaders)
	{
		Lock<> lock(getMutex());

		Vector<RendererMaterialData>& materials = getMaterials();
		for (UINT32 i = 0; i < materials.size(); i++)
			materials[i].metaData->shader = shaders[i];
	}

	void RendererMaterialManager::destroyOnCore()
	{
		Lock<> lock(getMutex());

		Vector<RendererMaterialData>& materials = getMaterials();
		for (UINT32 i = 0; i < materials.size(); i++)
			materials[i].metaData->shader = nullptr;
	}

	Vector<RendererMaterialManager::RendererMaterialData>& RendererMaterialManager::getMaterials()
	{
		static Vector<RendererMaterialData> materials;
		return materials;
	}

	Mutex& RendererMaterialManager::getMutex()
	{
		static Mutex mutex;
		return mutex;
	}
}