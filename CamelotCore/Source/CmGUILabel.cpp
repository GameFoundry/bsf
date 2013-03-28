#include "CmGUILabel.h"
#include "CmTextSprite.h"

namespace CamelotEngine
{
	GUILabel::GUILabel(const String& text, const FontPtr& font, UINT32 fontSize)
		:mText(text)
	{
		mTextSprite = new TextSprite(text, font, fontSize);
	}

	UINT32 GUILabel::getNumRenderElements() const
	{
		return mTextSprite->getNumRenderElements();
	}

	const MaterialHandle& GUILabel::getMaterial(UINT32 renderElementIdx) const
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
}