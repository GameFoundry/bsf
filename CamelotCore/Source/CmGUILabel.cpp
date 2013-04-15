#include "CmGUILabel.h"
#include "CmGUIElementStyle.h"
#include "CmTextSprite.h"
#include "CmGUISkin.h"

namespace CamelotEngine
{
	GUILabel::GUILabel(GUIWidget* parent, const String& text, const GUISkin* skin, UINT32 fixedWidth, UINT32 fixedHeight, 
		bool wordWrap, TextHorzAlign horzAlign, TextVertAlign vertAlign)
		:GUIElement(parent, skin), mText(text), mFixedWidth(fixedWidth), mFixedHeight(fixedHeight), mWordWrap(wordWrap),
		mHorzAlign(horzAlign), mVertAlign(vertAlign)
	{
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

	const String& GUILabel::getGUITypeName()
	{
		static String typeName = "Label";
		return typeName;
	}
}