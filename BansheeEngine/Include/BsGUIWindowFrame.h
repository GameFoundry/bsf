#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIWindowFrame : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIWindowFrame* create(GUIWidget& parent, const GUIElementStyle* style = nullptr);
		static GUIWindowFrame* create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style = nullptr);

		void setFocused(bool focused);
	protected:
		~GUIWindowFrame();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual CM::UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const CM::HMaterial& getMaterial(CM::UINT32 renderElementIdx) const;

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

		virtual CM::UINT32 _getOptimalWidth() const;
		virtual CM::UINT32 _getOptimalHeight() const;

		virtual bool _isInBounds(const CM::Int2 position) const;
	private:
		ImageSprite* mImageSprite;
		IMAGE_SPRITE_DESC mDesc;

		GUIWindowFrame(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);
	};
}