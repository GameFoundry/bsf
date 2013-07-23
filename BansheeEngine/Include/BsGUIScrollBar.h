#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"

#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIScrollBar : public GUIElement
	{
	public:
		boost::signal<void(float newPosition)> onScrollPositionChanged;

		void setHandleSize(CM::UINT32 size);
		void setScrollPos(float pct);

		/**
		 * @brief	Sets a page size that is used for determining how far to jump
		 * 			when clicking on the scrollbar empty space.
		 */
		void setPageSize(CM::UINT32 size);

		CM::UINT32 getMaxHandleSize() const;
		CM::UINT32 getScrollableSize() const;
	protected:
		GUIScrollBar(GUIWidget& parent, bool horizontal, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);
		virtual ~GUIScrollBar();

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

		virtual CM::UINT32 _getOptimalWidth() const;
		virtual CM::UINT32 _getOptimalHeight() const;

		virtual CM::UINT32 _getRenderElementDepth(CM::UINT32 renderElementIdx) const;
	private:
		GUILayout* mLayout;
		ImageSprite* mImageSprite;

		GUIButton* mUpBtn;
		GUIButton* mDownBtn;
		GUIScrollBarHandle* mHandleBtn;
		bool mHorizontal;

		static const CM::UINT32 ButtonScrollAmount;

		void handleMoved(float handlePct);
		void upButtonClicked();
		void downButtonClicked();
	};
}