#pragma once

#include "CmPrerequisites.h"
#include "CmSprite.h"

namespace CamelotEngine
{
	enum TextHorzAlign
	{
		THA_Left, THA_Center, THA_Right
	};

	enum TextVertAlign
	{
		TVA_Top, TVA_Center, TVA_Bottom
	};

	class CM_EXPORT TextSprite : public Sprite
	{
	public:
		TextSprite();
		TextSprite(const String& text, FontPtr font, UINT32 fontSize);

		void setText(const String& text) { mText = text; setDirty(); }
		void setFont(FontPtr font, UINT32 fontSize) { mFont = font; mFontSize = fontSize; setDirty(); }
		void setWordWrap(bool wordWrap) { mWordWrap = wordWrap; setDirty(); }
		void setAlignment(TextHorzAlign horzAlign, TextVertAlign vertAlign = TVA_Top) 
			{ mHorzAlign = horzAlign; mVertAlign = vertAlign; setDirty(); }

		String getText() const { return mText; }
		FontPtr getFont() const { return mFont; }
		UINT32 getFontSize() const { return mFontSize; }
		bool getWordWrap() const { return mWordWrap; }
		TextHorzAlign getTextHorzAlign() const { return mHorzAlign; }
		TextVertAlign getTextVertAlign() const { return mVertAlign; }

	protected:
		String mText;
		FontPtr mFont;
		UINT32 mFontSize;
		bool mWordWrap;
		TextHorzAlign mHorzAlign;
		TextVertAlign mVertAlign;

		virtual void updateMesh() const;

		const FontData* getFontData() const;

		void clipToRect(Vector2* vertices, Vector2* uv, UINT32 numQuads, Rect clipRect) const;
	};
}