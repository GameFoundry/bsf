#include "CmGUIMaterialManager.h"
#include "CmMaterial.h"
#include "CmDebug.h"
#include "CmBuiltinMaterialManager.h"

namespace CamelotEngine
{
	const MaterialHandle& GUIMaterialManager::requestTextMaterial(const TextureHandle& texture) const
	{
		for(auto& matHandle : mTextMaterials)
		{
			const MaterialHandle& material = matHandle.handle;
			if(material->getTexture("mainTexture") == texture)
			{
				matHandle.refCount++;
				return material;
			}
		}

		mTextMaterials.push_back(GUIMaterial());

		GUIMaterial& material = mTextMaterials[mTextMaterials.size() - 1];
		material.handle = BuiltinMaterialManager::instance().createSpriteTextMaterial();
		material.handle->setTexture("mainTexture", texture);
		material.refCount = 1;		

		return material.handle;
	}

	const MaterialHandle& GUIMaterialManager::requestImageMaterial(const TextureHandle& texture) const
	{
		for(auto& matHandle : mImageMaterials)
		{
			const MaterialHandle& material = matHandle.handle;
			if(material->getTexture("mainTexture") == texture)
			{
				matHandle.refCount++;
				return material;
			}
		}

		mImageMaterials.push_back(GUIMaterial());

		GUIMaterial& material = mImageMaterials[mImageMaterials.size() - 1];
		material.handle = BuiltinMaterialManager::instance().createSpriteImageMaterial();
		material.handle->setTexture("mainTexture", texture);
		material.refCount = 1;		

		return material.handle;
	}

	void GUIMaterialManager::releaseMaterial(const MaterialHandle& material) const
	{
		bool released = false;

		UINT32 i = 0;
		for(auto& matHandle : mTextMaterials)
		{
			if(matHandle.handle == material)
			{
				if(--matHandle.refCount == 0)
				{
					mTextMaterials.erase(mTextMaterials.begin() + i);
					released = true;
					break;
				}
			}

			i++;
		}

		i = 0;
		for(auto& matHandle : mImageMaterials)
		{
			if(matHandle.handle == material)
			{
				if(--matHandle.refCount == 0)
				{
					mImageMaterials.erase(mImageMaterials.begin() + i);
					released = true;
					break;
				}
			}

			i++;
		}

		if(!released)
		{
			LOGWRN("Trying to release a material that doesn't exist in the GUI Manager. Material ID: " + toString(material->getInternalID()));
		}
	}
}