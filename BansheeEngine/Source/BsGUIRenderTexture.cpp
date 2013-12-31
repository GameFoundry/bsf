#include "BsGUIRenderTexture.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUIManager.h"
#include "CmRenderTexture.h"
#include "BsSpriteTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIRenderTexture::getGUITypeName()
	{
		static String name = "RenderTexture";
		return name;
	}

	GUIRenderTexture::GUIRenderTexture(GUIWidget& parent, const GUIElementStyle* style, const RenderTexturePtr& texture, const GUILayoutOptions& layoutOptions)
		:GUITexture(parent, style, HSpriteTexture(), GUIImageScaleMode::StretchToFit, layoutOptions), mSourceTexture(texture.get())
	{
		if(mSourceTexture->requiresTextureFlipping())
		{
			mDesc.uvOffset = Vector2(0.0f, 1.0f);
			mDesc.uvScale = Vector2(1.0f, -1.0f);
		}

		setTexture(SpriteTexture::create(texture->getBindableColorTexture()));
		GUIManager::instance().setInputBridge(mSourceTexture, this);
	}

	GUIRenderTexture::~GUIRenderTexture()
	{
		GUIManager::instance().setInputBridge(mSourceTexture, nullptr);
	}

	GUIRenderTexture* GUIRenderTexture::create(GUIWidget& parent, const RenderTexturePtr& texture, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIRenderTexture, PoolAlloc>()) GUIRenderTexture(parent, style, texture, GUILayoutOptions::create(style));
	}

	GUIRenderTexture* GUIRenderTexture::create(GUIWidget& parent, const RenderTexturePtr& texture, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIRenderTexture, PoolAlloc>()) GUIRenderTexture(parent, style, texture, GUILayoutOptions::create(layoutOptions, style));
	}

	void GUIRenderTexture::updateRenderElementsInternal()
	{		
		if(mActiveTexture != nullptr && mActiveTexture.isLoaded())
		{
			mDesc.texture = mActiveTexture.getInternalPtr();
		}

		mDesc.width = mWidth;
		mDesc.height = mHeight;

		mImageSprite->update(mDesc);

		GUIElement::updateRenderElementsInternal();
	}
}