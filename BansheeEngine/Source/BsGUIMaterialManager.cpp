#include "BsGUIMaterialManager.h"
#include "BsMaterial.h"
#include "BsSamplerState.h"
#include "BsDebug.h"
#include "BsBuiltinResources.h"
#include "BsColor.h"

namespace BansheeEngine
{
	GUIMaterialManager::GUIMaterialManager()
	{
		SAMPLER_STATE_DESC ssDesc;
		ssDesc.magFilter = FO_POINT;
		ssDesc.minFilter = FO_POINT;
		ssDesc.mipFilter = FO_POINT;
		mGUISamplerState = SamplerState::create(ssDesc);
	}

	const GUIMaterialInfo& GUIMaterialManager::requestTextMaterial(const HTexture& texture, const Color& tint) const
	{
		Vector4 vecColor(tint.r, tint.g, tint.b, tint.a);

		const GUIMaterialInfo* matInfo = findExistingTextMaterial(texture, tint);
		if(matInfo != nullptr)
			return *matInfo;

		mTextMaterials.push_back(GUIMaterial());

		GUIMaterial& guiMat = mTextMaterials[mTextMaterials.size() - 1];
		guiMat.handle = BuiltinResources::instance().createSpriteTextMaterial();

		guiMat.handle.mainTexSampler.set(mGUISamplerState);
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
		guiMat.handle = BuiltinResources::instance().createSpriteImageMaterial();

		guiMat.handle.mainTexSampler.set(mGUISamplerState);
		guiMat.handle.mainTexture.set(texture);
		guiMat.handle.tint.set(vecColor);
		guiMat.refCount = 1;		

		return guiMat.handle;
	}

	const GUIMaterialInfo& GUIMaterialManager::requestNonAlphaImageMaterial(const HTexture& texture, const Color& tint) const
	{
		Vector4 vecColor(tint.r, tint.g, tint.b, tint.a);

		const GUIMaterialInfo* matInfo = findExistingNonAlphaImageMaterial(texture, tint);
		if (matInfo != nullptr)
			return *matInfo;

		mNonAlphaImageMaterials.push_back(GUIMaterial());

		GUIMaterial& guiMat = mNonAlphaImageMaterials[mNonAlphaImageMaterials.size() - 1];
		guiMat.handle = BuiltinResources::instance().createSpriteNonAlphaImageMaterial();

		guiMat.handle.mainTexSampler.set(mGUISamplerState);
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

	const GUIMaterialInfo* GUIMaterialManager::findExistingNonAlphaImageMaterial(const HTexture& texture, const Color& tint) const
	{
		Vector4 vecColor(tint.r, tint.g, tint.b, tint.a);

		for (auto& matHandle : mNonAlphaImageMaterials)
		{
			if (matHandle.handle.mainTexture.get() == texture && matHandle.handle.tint.get() == vecColor)
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

	void GUIMaterialManager::clearMaterials()
	{
		mTextMaterials.clear();
		mImageMaterials.clear();
	}
}