#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsGUIContent.h"

namespace BansheeEditor
{
	class GUIColor : public BS::GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIColor* create(BS::GUIWidget& parent, const BS::GUIElementStyle* style = nullptr);
		static GUIColor* create(BS::GUIWidget& parent, const BS::GUIOptions& layoutOptions, const BS::GUIElementStyle* style = nullptr);

		virtual CM::Vector2I _getOptimalSize() const;

		void setColor(const CM::Color& color);
		CM::Color getColor() const { return mColor; }

	protected:
		GUIColor(BS::GUIWidget& parent, const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions);
		virtual ~GUIColor();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual CM::UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const BS::GUIMaterialInfo& getMaterial(CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads()
		 */
		virtual CM::UINT32 getNumQuads(CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer()
		 */
		virtual void fillBuffer(CM::UINT8* vertices, CM::UINT8* uv, CM::UINT32* indices, CM::UINT32 startingQuad, 
			CM::UINT32 maxNumQuads, CM::UINT32 vertexStride, CM::UINT32 indexStride, CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();

		/**
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);

	private:
		static const float ALPHA_SPLIT_POSITION;

		BS::ImageSprite* mColorSprite;
		BS::ImageSprite* mAlphaSprite;

		BS::IMAGE_SPRITE_DESC mColorImageDesc;
		BS::IMAGE_SPRITE_DESC mAlphaImageDesc;

		CM::Color mColor;
	};
}