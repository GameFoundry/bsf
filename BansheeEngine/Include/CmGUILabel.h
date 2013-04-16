#pragma once

#include "BsPrerequisites.h"
#include "CmGUIElement.h"
#include "CmTextSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUILabel : public CM::GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUILabel* create(CM::GUIWidget* parent, const CM::String& text);
		static GUILabel* create(CM::GUIWidget* parent, const CM::String& text, CM::TextHorzAlign horzAlign, CM::TextVertAlign vertAlign);
		static GUILabel* create(CM::GUIWidget* parent, const CM::String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap);
		static GUILabel* create(CM::GUIWidget* parent, const CM::String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap, CM::TextHorzAlign horzAlign, CM::TextVertAlign vertAlign);

	protected:
		~GUILabel();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const CM::HMaterial& getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads()
		 */
		virtual UINT32 getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer()
		 */
		virtual void fillBuffer(CM::Vector2* vertices, CM::Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, UINT32 renderElementIdx) const;
	private:
		CM::TextSprite* mTextSprite;
		CM::String mText;
		UINT32 mFixedWidth, mFixedHeight;
		bool mWordWrap;
		CM::TextHorzAlign mHorzAlign;
		CM::TextVertAlign mVertAlign;

		GUILabel(CM::GUIWidget* parent, const CM::String& text, UINT32 fixedWidth, UINT32 fixedHeight, 
			bool wordWrap, CM::TextHorzAlign horzAlign, CM::TextVertAlign vertAlign);
	};
}