#include "BsGUIRenderTexture.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUIManager.h"
#include "BsRenderTexture.h"
#include "BsSpriteTexture.h"

namespace BansheeEngine
{
	const String& GUIRenderTexture::getGUITypeName()
	{
		static String name = "RenderTexture";
		return name;
	}

	GUIRenderTexture::GUIRenderTexture(const String& styleName, const RenderTexturePtr& texture, const GUILayoutOptions& layoutOptions)
		:GUITexture(styleName, HSpriteTexture(), GUIImageScaleMode::StretchToFit, false, layoutOptions)
	{
		setRenderTexture(texture);
	}

	GUIRenderTexture::~GUIRenderTexture()
	{
		if (mSourceTexture != nullptr)
			GUIManager::instance().setInputBridge(mSourceTexture.get(), nullptr);
	}

	GUIRenderTexture* GUIRenderTexture::create(const RenderTexturePtr& texture, const String& styleName)
	{
		return new (bs_alloc<GUIRenderTexture, PoolAlloc>()) GUIRenderTexture(getStyleName<GUIRenderTexture>(styleName), texture, GUILayoutOptions::create());
	}

	GUIRenderTexture* GUIRenderTexture::create(const RenderTexturePtr& texture, const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (bs_alloc<GUIRenderTexture, PoolAlloc>()) GUIRenderTexture(getStyleName<GUIRenderTexture>(styleName), texture, GUILayoutOptions::create(layoutOptions));
	}

	void GUIRenderTexture::setRenderTexture(const RenderTexturePtr& texture)
	{
		if (mSourceTexture != nullptr)
			GUIManager::instance().setInputBridge(mSourceTexture.get(), nullptr);

		mSourceTexture = texture;

		if (mSourceTexture != nullptr)
		{
			if (mSourceTexture->requiresTextureFlipping())
			{
				mDesc.uvOffset = Vector2(0.0f, 1.0f);
				mDesc.uvScale = Vector2(1.0f, -1.0f);
			}

			setTexture(SpriteTexture::create(texture->getBindableColorTexture()));

			GUIManager::instance().setInputBridge(mSourceTexture.get(), this);
		}
		else
		{
			setTexture(SpriteTexture::create(HTexture()));
		}

		markContentAsDirty();
	}

	void GUIRenderTexture::updateRenderElementsInternal()
	{		
		if(mActiveTexture != nullptr && mActiveTexture.isLoaded())
		{
			mDesc.texture = mActiveTexture.getInternalPtr();
		}

		mDesc.width = mWidth;
		mDesc.height = mHeight;
		mDesc.transparent = false;

		mImageSprite->update(mDesc);

		GUIElement::updateRenderElementsInternal();
	}
}