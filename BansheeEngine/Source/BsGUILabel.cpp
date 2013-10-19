#include "BsGUILabel.h"
#include "BsGUIElementStyle.h"
#include "BsTextSprite.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIHelper.h"
#include "CmTextUtility.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUILabel::GUILabel(GUIWidget& parent, const GUIElementStyle* style, const GUIContent& content, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mContent(content)
	{
		mTextSprite = cm_new<TextSprite, PoolAlloc>();

		mDesc.font = mStyle->font;
		mDesc.fontSize = mStyle->fontSize;
		mDesc.wordWrap = mStyle->wordWrap;
		mDesc.horzAlign = mStyle->textHorzAlign;
		mDesc.vertAlign = mStyle->textVertAlign;

		mLocStringUpdatedConn = mContent.getText().addOnStringModifiedCallback(boost::bind(&GUILabel::updateRenderElementsInternal, this));
	}

	GUILabel::~GUILabel()
	{
		mLocStringUpdatedConn.disconnect();
		cm_delete<PoolAlloc>(mTextSprite);
	}

	UINT32 GUILabel::getNumRenderElements() const
	{
		return mTextSprite->getNumRenderElements();
	}

	const HMaterial& GUILabel::getMaterial(UINT32 renderElementIdx) const
	{
		return mTextSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUILabel::getNumQuads(UINT32 renderElementIdx) const
	{
		return mTextSprite->getNumQuads(renderElementIdx);
	}

	void GUILabel::updateRenderElementsInternal()
	{		
		mDesc.width = mWidth;
		mDesc.height = mHeight;
		mDesc.text = mContent.getText();

		mTextSprite->update(mDesc);

		GUIElement::updateRenderElementsInternal();
	}

	void GUILabel::updateClippedBounds()
	{
		mClippedBounds = mTextSprite->getBounds(mOffset, mClipRect);
	}

	Int2 GUILabel::_getOptimalSize() const
	{
		return GUIHelper::calcOptimalContentsSize(mContent, *mStyle, _getLayoutOptions());
	}

	void GUILabel::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mTextSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
	}

	void GUILabel::setContent(const GUIContent& content)
	{
		mLocStringUpdatedConn.disconnect();
		mLocStringUpdatedConn = content.getText().addOnStringModifiedCallback(boost::bind(&GUILabel::updateRenderElementsInternal, this));

		mContent = content;
		
		markContentAsDirty();
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const HString& text, const GUIElementStyle* style)
	{
		return create(parent, GUIContent(text), style);
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const HString& text, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return create(parent, GUIContent(text), layoutOptions, style);
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const GUIContent& content, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUILabel, PoolAlloc>()) GUILabel(parent, style, content, GUILayoutOptions::create(style));
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const GUIContent& content, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUILabel, PoolAlloc>()) GUILabel(parent, style, content, GUILayoutOptions::create(layoutOptions, style));
	}

	const String& GUILabel::getGUITypeName()
	{
		static String typeName = "Label";
		return typeName;
	}
}