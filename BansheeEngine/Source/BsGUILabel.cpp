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
	GUILabel::GUILabel(GUIWidget& parent, const String& text, bool wordWrap, TextHorzAlign horzAlign, 
		TextVertAlign vertAlign, const GUI_LAYOUT_OPTIONS& layoutOptions)
		:GUIElement(parent, layoutOptions), mText(text), mWordWrap(wordWrap),
		mHorzAlign(horzAlign), mVertAlign(vertAlign)
	{
		const GUISkin* skin = parent.getGUISkin();

		mStyle = skin->getStyle(getGUITypeName());
		mTextSprite = CM_NEW(TextSprite, PoolAlloc) TextSprite();

		mDesc.text = text;
		mDesc.font = mStyle->font;
		mDesc.fontSize = mStyle->fontSize;
		mDesc.wordWrap = wordWrap;
		mDesc.horzAlign = horzAlign;
		mDesc.vertAlign = vertAlign;
	}

	GUILabel::~GUILabel()
	{
		CM_DELETE(mTextSprite, TextSprite, PoolAlloc);
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

	UINT32 GUILabel::getOptimalWidth() const
	{
		UINT32 wordWrapWidth = 0;

		if(mDesc.wordWrap)
		{
			if(getLayoutOptions().fixedWidth)
				wordWrapWidth = getLayoutOptions().width;
			else if(getLayoutOptions().maxWidth > 0)
				wordWrapWidth = getLayoutOptions().maxWidth;
		}

		std::shared_ptr<TextUtility::TextData> textData = TextUtility::getTextData(mDesc.text, mDesc.font, mDesc.fontSize, wordWrapWidth, 0, mDesc.wordWrap);
		if(textData == nullptr)
			return 0;

		return textData->getWidth();
	}

	UINT32 GUILabel::getOptimalHeight() const
	{
		UINT32 wordWrapWidth = 0;

		if(mDesc.wordWrap)
		{
			if(getLayoutOptions().fixedWidth)
				wordWrapWidth = getLayoutOptions().width;
			else if(getLayoutOptions().maxWidth > 0)
				wordWrapWidth = getLayoutOptions().maxWidth;
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

	const GUI_LAYOUT_OPTIONS& GUILabel::getDefaultLayoutOptions()
	{
		static GUI_LAYOUT_OPTIONS layoutOptions;
		static bool layoutOptionsInitialized = false;

		if(!layoutOptionsInitialized)
		{
			layoutOptions.fixedWidth = false;
			layoutOptions.fixedHeight = true;
			layoutOptions.height = 20;
			layoutOptions.minWidth = 10;

			layoutOptionsInitialized = true;
		}

		return layoutOptions;
	}

	void GUILabel::setText(const CM::String& text)
	{
		mDesc.text = text;

		mTextSprite->update(mDesc);
		mBounds = mTextSprite->getBounds();

		markAsDirty();
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, bool wordWrap)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, wordWrap, THA_Left, TVA_Top, getDefaultLayoutOptions());
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, const GUI_LAYOUT_OPTIONS& layoutOptions, bool wordWrap)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, wordWrap, THA_Left, TVA_Top, layoutOptions);
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, TextHorzAlign horzAlign, TextVertAlign vertAlign, bool wordWrap)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, wordWrap, horzAlign, vertAlign, getDefaultLayoutOptions());
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, const GUI_LAYOUT_OPTIONS& layoutOptions, TextHorzAlign horzAlign, TextVertAlign vertAlign, bool wordWrap)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, wordWrap, horzAlign, vertAlign, layoutOptions);
	}

	const String& GUILabel::getGUITypeName()
	{
		static String typeName = "Label";
		return typeName;
	}
}