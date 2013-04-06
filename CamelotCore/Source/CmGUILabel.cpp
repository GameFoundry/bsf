#include "CmGUILabel.h"
#include "CmGUIElementStyle.h"
#include "CmTextSprite.h"
#include "CmGUISkin.h"

namespace CamelotEngine
{
	GUILabel::GUILabel(GUIWidget* parent, const String& text, const GUISkin* skin)
		:GUIElement(parent, skin), mText(text)
	{
		// This is calling a virtual method but it's okay because we always want the one
		// existing on this class.
		mStyle = skin->getStyle(getGUITypeName());
		mTextSprite = new TextSprite(text, mStyle->font, mStyle->fontSize);
	}

	GUILabel::~GUILabel()
	{
		delete mTextSprite;
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