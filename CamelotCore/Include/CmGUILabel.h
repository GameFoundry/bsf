#pragma once

#include "CmPrerequisites.h"
#include "CmGUIElement.h"
#include "CmTextSprite.h"

namespace CamelotEngine
{
	class CM_EXPORT GUILabel : public GUIElement
	{
	public:
		static const String& getGUITypeName();

		static GUILabel* create(GUIWidget* parent, const String& text);
		static GUILabel* create(GUIWidget* parent, const String& text, TextHorzAlign horzAlign, TextVertAlign vertAlign);
		static GUILabel* create(GUIWidget* parent, const String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap);
		static GUILabel* create(GUIWidget* parent, const String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap, TextHorzAlign horzAlign, TextVertAlign vertAlign);

	protected:
		~GUILabel();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const HMaterial& getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads()
		 */
		virtual UINT32 getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer()
		 */
		virtual void fillBuffer(Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, UINT32 renderElementIdx) const;
	private:
		TextSprite* mTextSprite;
		String mText;
		UINT32 mFixedWidth, mFixedHeight;
		bool mWordWrap;
		TextHorzAlign mHorzAlign;
		TextVertAlign mVertAlign;

		GUILabel(GUIWidget* parent, const String& text, UINT32 fixedWidth, UINT32 fixedHeight, 
			bool wordWrap, TextHorzAlign horzAlign, TextVertAlign vertAlign);
	};
}