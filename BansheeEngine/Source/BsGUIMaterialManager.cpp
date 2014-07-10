//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGUIMaterialManager.h"
#include "BsMaterial.h"
#include "BsDebug.h"
#include "BsBuiltinMaterialManager.h"
#include "BsColor.h"

namespace BansheeEngine
{
	const GUIMaterialInfo& GUIMaterialManager::requestTextMaterial(const HTexture& texture, const Color& tint) const
	{
		Vector4 vecColor(tint.r, tint.g, tint.b, tint.a);

		const GUIMaterialInfo* matInfo = findExistingTextMaterial(texture, tint);
		if(matInfo != nullptr)
			return *matInfo;

		mTextMaterials.push_back(GUIMaterial());

		GUIMaterial& guiMat = mTextMaterials[mTextMaterials.size() - 1];
		guiMat.handle = BuiltinMaterialManager::instance().createSpriteTextMaterial();

		guiMat.handle.mainTexture.set(texture);
		guiMat.handle.tint.set(vecColor);
		guiMat.refCount = 1;		

		return guiMat.handle;
	}

	const GUIMaterialInfo& GUIMaterialManager::requestImageMaterial(const HTexture& texture, const Color& tint) const
	{
		Vector4 vecColor(tint.r, tint.g, tint.b, tint.a);

		const GUIMaterialInfo* matInfo = findExistingImageMaterial(texture, tint);
		if(matInfo != nullptr)
			return *matInfo;

		mImageMaterials.push_back(GUIMaterial());

		GUIMaterial& guiMat = mImageMaterials[mImageMaterials.size() - 1];
		guiMat.handle = BuiltinMaterialManager::instance().createSpriteImageMaterial();

		guiMat.handle.mainTexture.set(texture);
		guiMat.handle.tint.set(vecColor);
		guiMat.refCount = 1;		

		return guiMat.handle;
	}

	const GUIMaterialInfo* GUIMaterialManager::findExistingTextMaterial(const HTexture& texture, const Color& tint) const
	{
		Vector4 vecColor(tint.r, tint.g, tint.b, tint.a);

		for(auto& matHandle : mTextMaterials)
		{
			if(matHandle.handle.mainTexture.get() == texture && matHandle.handle.tint.get() == vecColor)
			{
				matHandle.refCount++;
				return &matHandle.handle;
			}
		}

		return nullptr;
	}

	const GUIMaterialInfo* GUIMaterialManager::findExistingImageMaterial(const HTexture& texture, const Color& tint) const
	{
		Vector4 vecColor(tint.r, tint.g, tint.b, tint.a);

		for(auto& matHandle : mImageMaterials)
		{
			if(matHandle.handle.mainTexture.get() == texture && matHandle.handle.tint.get() == vecColor)
			{
				matHandle.refCount++;
				return &matHandle.handle;
			}
		}

		return nullptr;
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