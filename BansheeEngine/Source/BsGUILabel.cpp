#include "BsGUILabel.h"
#include "BsGUIElementStyle.h"
#include "BsTextSprite.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"

using namespace CamelotEngine;

namespace BansheeEngine
{
	GUILabel::GUILabel(GUIWidget* parent, const String& text, UINT32 fixedWidth, UINT32 fixedHeight, 
		bool wordWrap, TextHorzAlign horzAlign, TextVertAlign vertAlign)
		:GUIElement(parent), mText(text), mFixedWidth(fixedWidth), mFixedHeight(fixedHeight), mWordWrap(wordWrap),
		mHorzAlign(horzAlign), mVertAlign(vertAlign)
	{
		const GUISkin* skin = parent->getGUISkin();

		mStyle = skin->getStyle(getGUITypeName());
		mTextSprite = CM_NEW(TextSprite, PoolAlloc) TextSprite();

		TEXT_SPRITE_DESC desc;
		desc.text = text;
		desc.font = mStyle->font;
		desc.fontSize = mStyle->fontSize;
		desc.width = fixedWidth;
		desc.height = fixedHeight;
		desc.wordWrap = wordWrap;
		desc.clipRect = Rect(0, 0, fixedWidth, fixedHeight);
		desc.horzAlign = horzAlign;
		desc.vertAlign = vertAlign;
		mTextSprite->update(desc);

		mBounds = mTextSprite->getBounds();
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

	void GUILabel::fillBuffer(Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, UINT32 renderElementIdx) const
	{
		mTextSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, renderElementIdx);
	}

	GUILabel* GUILabel::create(GUIWidget* parent, const String& text)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, 0, 0, false, THA_Left, TVA_Top);
	}

	GUILabel* GUILabel::create(GUIWidget* parent, const String& text, TextHorzAlign horzAlign, TextVertAlign vertAlign)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, 0, 0, false, horzAlign, vertAlign);
	}

	GUILabel* GUILabel::create(GUIWidget* parent, const String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, fixedWidth, fixedHeight, wordWrap, THA_Left, TVA_Top);
	}

	GUILabel* GUILabel::create(GUIWidget* parent, const String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap, TextHorzAlign horzAlign, TextVertAlign vertAlign)
	{
		return CM_NEW(GUILabel, PoolAlloc) GUILabel(parent, text, fixedWidth, fixedHeight, wordWrap, horzAlign, vertAlign);
	}

	const String& GUILabel::getGUITypeName()
	{
		static String typeName = "Label";
		return typeName;
	}
}