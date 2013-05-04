#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUILabel : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUILabel* create(GUIWidget* parent, const CM::String& text);
		static GUILabel* create(GUIWidget* parent, const CM::String& text, TextHorzAlign horzAlign, TextVertAlign vertAlign);
		static GUILabel* create(GUIWidget* parent, const CM::String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap);
		static GUILabel* create(GUIWidget* parent, const CM::String& text, UINT32 fixedWidth, UINT32 fixedHeight, bool wordWrap, TextHorzAlign horzAlign, TextVertAlign vertAlign);

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
		virtual void fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const;
	private:
		TextSprite* mTextSprite;
		CM::String mText;
		UINT32 mFixedWidth, mFixedHeight;
		bool mWordWrap;
		TextHorzAlign mHorzAlign;
		TextVertAlign mVertAlign;

		GUILabel(GUIWidget* parent, const CM::String& text, UINT32 fixedWidth, UINT32 fixedHeight, 
			bool wordWrap, TextHorzAlign horzAlign, TextVertAlign vertAlign);
	};
}