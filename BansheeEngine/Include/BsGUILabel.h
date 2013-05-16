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

		static GUILabel* create(GUIWidget& parent, const CM::String& text, bool wordWrap = false);
		static GUILabel* create(GUIWidget& parent, const CM::String& text, const GUI_LAYOUT_OPTIONS& layoutOptions, bool wordWrap = false);

		static GUILabel* create(GUIWidget& parent, const CM::String& text, TextHorzAlign horzAlign, 
			TextVertAlign vertAlign = TVA_Top, bool wordWrap = false);
		static GUILabel* create(GUIWidget& parent, const CM::String& text, const GUI_LAYOUT_OPTIONS& layoutOptions,
			TextHorzAlign horzAlign, TextVertAlign vertAlign = TVA_Top, bool wordWrap = false);

		void setText(const CM::String& text);

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

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();

		static const GUI_LAYOUT_OPTIONS& getDefaultLayoutOptions();
	private:
		TextSprite* mTextSprite;
		CM::String mText;
		bool mWordWrap;
		TextHorzAlign mHorzAlign;
		TextVertAlign mVertAlign;
		TEXT_SPRITE_DESC mDesc;
		
		GUILabel(GUIWidget& parent, const CM::String& text, bool wordWrap, TextHorzAlign horzAlign, TextVertAlign vertAlign, 
			const GUI_LAYOUT_OPTIONS& layoutOptions);
	};
}