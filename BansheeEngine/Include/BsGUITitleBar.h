#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUITitleBar : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUITitleBar* create(GUIWidget& parent, const CM::String& title, const GUIElementStyle* style = nullptr);
		static GUITitleBar* create(GUIWidget& parent, const CM::String& title, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style = nullptr);
	protected:
		~GUITitleBar();

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

		GUITitleBar(GUIWidget& parent, const GUIElementStyle* style, const CM::String& title, const GUILayoutOptions& layoutOptions);
	};
}