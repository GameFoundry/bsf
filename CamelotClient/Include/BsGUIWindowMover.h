#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include <boost/signals.hpp>

namespace BansheeEditor
{
	class GUIWindowMover : public BS::GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIWindowMover* create(BS::GUIWidget& parent, const BS::GUIElementStyle* style = nullptr);
		static GUIWindowMover* create(BS::GUIWidget& parent, const BS::GUILayoutOptions& layoutOptions, const BS::GUIElementStyle* style = nullptr);

		void setFocused(bool focused);

		boost::signal<void()> onDraggedItemDropped;
	protected:
		~GUIWindowMover();

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
		virtual void updateBounds();

		virtual CM::UINT32 _getOptimalWidth() const;
		virtual CM::UINT32 _getOptimalHeight() const;
	private:
		BS::ImageSprite* mImageSprite;
		BS::IMAGE_SPRITE_DESC mDesc;

		GUIWindowMover(BS::GUIWidget& parent, const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);
	};
}