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
		:GUITexture(parent, style, nullptr, GUIImageScaleMode::StretchToFit, layoutOptions), mSourceTexture(texture.get())
	{
		setTexture(std::make_shared<SpriteTexture>(texture->getBindableColorTexture()));
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
}