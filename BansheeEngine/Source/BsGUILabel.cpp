#include "BsGUILabel.h"
#include "BsGUIElementStyle.h"
#include "BsTextSprite.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutOptions.h"
#include "CmTextUtility.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUILabel::GUILabel(GUIWidget& parent, const GUIElementStyle* style, const String& text, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mText(text)
	{
		mTextSprite = cm_new<TextSprite, PoolAlloc>();

		mDesc.text = text;
		mDesc.font = mStyle->font;
		mDesc.fontSize = mStyle->fontSize;
		mDesc.wordWrap = mStyle->wordWrap;
		mDesc.horzAlign = mStyle->textHorzAlign;
		mDesc.vertAlign = mStyle->textVertAlign;
	}

	GUILabel::~GUILabel()
	{
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
		mDesc.offset = mOffset;
		mDesc.width = mWidth;
		mDesc.height = mHeight;
		mDesc.clipRect = mClipRect;

		mTextSprite->update(mDesc);
		mBounds = mTextSprite->getBounds();
	}

	UINT32 GUILabel::_getOptimalWidth() const
	{
		UINT32 wordWrapWidth = 0;

		if(mDesc.wordWrap)
		{
			if(_getLayoutOptions().fixedWidth)
				wordWrapWidth = _getLayoutOptions().width;
			else
				wordWrapWidth = _getLayoutOptions().maxWidth;
		}

		std::shared_ptr<TextUtility::TextData> textData = TextUtility::getTextData(mDesc.text, mDesc.font, mDesc.fontSize, wordWrapWidth, 0, mDesc.wordWrap);
		if(textData == nullptr)
			return 0;

		return textData->getWidth();
	}

	UINT32 GUILabel::_getOptimalHeight() const
	{
		UINT32 wordWrapWidth = 0;

		if(mDesc.wordWrap)
		{
			if(_getLayoutOptions().fixedWidth)
				wordWrapWidth = _getLayoutOptions().width;
			else
				wordWrapWidth = _getLayoutOptions().maxWidth;
		}

		std::shared_ptr<TextUtility::TextData> textData = TextUtility::getTextData(mDesc.text, mDesc.font, mDesc.fontSize, wordWrapWidth, 0, mDesc.wordWrap);
		if(textData == nullptr)
			return 0;

		return textData->getHeight();
	}

	void GUILabel::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mTextSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx);
	}

	void GUILabel::setText(const CM::String& text)
	{
		mDesc.text = text;

		mTextSprite->update(mDesc);
		mBounds = mTextSprite->getBounds();

		markAsDirty();
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getGUISkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUILabel, PoolAlloc>()) GUILabel(parent, style, text, getDefaultLayoutOptions(style));
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getGUISkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUILabel, PoolAlloc>()) GUILabel(parent, style, text, layoutOptions);
	}

	const String& GUILabel::getGUITypeName()
	{
		static String typeName = "Label";
		return typeName;
	}
}