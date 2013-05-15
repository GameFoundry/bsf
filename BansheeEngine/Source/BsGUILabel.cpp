#include "BsGUILabel.h"
#include "BsGUIElementStyle.h"
#include "BsTextSprite.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutOptions.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUILabel::GUILabel(GUIWidget& parent, const String& text, UINT32 fixedWidth, UINT32 fixedHeight, 
		bool wordWrap, TextHorzAlign horzAlign, TextVertAlign vertAlign, const GUI_LAYOUT_OPTIONS* layoutOptions)
		:GUIElement(parent), mText(text), mFixedWidth(fixedWidth), mFixedHeight(fixedHeight), mWordWrap(wordWrap),
		mHorzAlign(horzAlign), mVertAlign(vertAlign)
	{
		const GUISkin* skin = parent.getGUISkin();

		mStyle = skin->getStyle(getGUITypeName());
		mTextSprite = CM_NEW(TextSprite, PoolAlloc) TextSprite();

		mDesc.text = text;
		mDesc.font = mStyle->font;
		mDesc.fontSize = mStyle->fontSize;
		mDesc.width = fixedWidth;
		mDesc.height = fixedHeight;
		mDesc.wordWrap = wordWrap;
		mDesc.clipRect = Rect(0, 0, fixedWidth, fixedHeight);
		mDesc.horzAlign = horzAlign;
		mDesc.vertAlign = vertAlign;
		mTextSprite->update(mDesc);

		mBounds = mTextSprite->getBounds();

		if(layoutOptions != nullptr)
			mLayoutOptions = *layoutOptions;
		else
			mLayoutOptions = getDefaultLayoutOptions();
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

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, const GUI_LAYOUT_OPTIONS* layoutOptions)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, 0, 0, false, THA_Left, TVA_Top, layoutOptions);
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, TextHorzAlign horzAlign, TextVertAlign vertAlign, 
		const GUI_LAYOUT_OPTIONS* layoutOptions)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, 0, 0, false, horzAlign, vertAlign, layoutOptions);
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap, 
		const GUI_LAYOUT_OPTIONS* layoutOptions)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, fixedWidth, fixedHeight, wordWrap, THA_Left, TVA_Top, layoutOptions);
	}

	GUILabel* GUILabel::create(GUIWidget& parent, const String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap, 
		TextHorzAlign horzAlign, TextVertAlign vertAlign, const GUI_LAYOUT_OPTIONS* layoutOptions)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, fixedWidth, fixedHeight, wordWrap, horzAlign, vertAlign, layoutOptions);
	}

	const String& GUILabel::getGUITypeName()
	{
		static String typeName = "Label";
		return typeName;
	}
}