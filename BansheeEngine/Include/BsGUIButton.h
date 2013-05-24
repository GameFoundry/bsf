#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIButton : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIButton* create(GUIWidget& parent, const CM::String& text);
		static GUIButton* create(GUIWidget& parent, const CM::String& text, const GUILayoutOptions& layoutOptions);
	protected:
		~GUIButton();

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

		virtual UINT32 _getOptimalWidth() const;
		virtual UINT32 _getOptimalHeight() const;

		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const;
	private:
		ImageSprite* mImageSprite;
		TextSprite* mTextSprite;
		UINT32 mNumImageRenderElements;

		IMAGE_SPRITE_DESC mImageDesc;
		CM::String mText;

		GUIButton(GUIWidget& parent, const CM::String& text, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);
	};
}