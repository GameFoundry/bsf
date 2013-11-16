#include "BsGUIMaterialManager.h"
#include "CmMaterial.h"
#include "CmDebug.h"
#include "BsBuiltinMaterialManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const GUIMaterialInfo& GUIMaterialManager::requestTextMaterial(const HTexture& texture) const
	{
		for(auto& matHandle : mTextMaterials)
		{
			if(matHandle.handle.mainTexture.get() == texture)
			{
				matHandle.refCount++;
				return matHandle.handle;
			}
		}

		mTextMaterials.push_back(GUIMaterial());

		GUIMaterial& guiMat = mTextMaterials[mTextMaterials.size() - 1];
		guiMat.handle = BuiltinMaterialManager::instance().createSpriteTextMaterial();

		guiMat.handle.mainTexture.set(texture);
		guiMat.refCount = 1;		

		return guiMat.handle;
	}

	const GUIMaterialInfo& GUIMaterialManager::requestImageMaterial(const HTexture& texture) const
	{
		for(auto& matHandle : mImageMaterials)
		{
			if(matHandle.handle.mainTexture.get() == texture)
			{
				matHandle.refCount++;
				return matHandle.handle;
			}
		}

		mImageMaterials.push_back(GUIMaterial());

		GUIMaterial& guiMat = mImageMaterials[mImageMaterials.size() - 1];
		guiMat.handle = BuiltinMaterialManager::instance().createSpriteImageMaterial();

		guiMat.handle.mainTexture.set(texture);
		guiMat.refCount = 1;		

		return guiMat.handle;
	}

	void GUIMaterialManager::releaseMaterial(const GUIMaterialInfo& material) const
	{
		bool released = false;

		UINT32 i = 0;
		for(auto& matHandle : mTextMaterials)
		{
			if(&matHandle.handle == &material)
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
			if(&matHandle.handle == &material)
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
	}

	void GUIMaterialManager::forceReleaseAllMaterials()
	{
		mTextMaterials.clear();
		mImageMaterials.clear();
	}
}