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

		float getScrollPos() const;

		/**
		 * @brief	Scrolls the contents by some amount. Amount is specified in the percentage
		 * 			of the entire scrollable area. Values out of range will be clamped.
		 */
		void scroll(float amount);

		CM::UINT32 getMaxHandleSize() const;
		CM::UINT32 getScrollableSize() const;

		virtual CM::Vector2I _getOptimalSize() const;
	protected:
		GUIScrollBar(bool horizontal, const CM::String& styleName, const GUILayoutOptions& layoutOptions);
		virtual ~GUIScrollBar();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual CM::UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const GUIMaterialInfo& getMaterial(CM::UINT32 renderElementIdx) const;

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