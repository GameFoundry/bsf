#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIScrollBar : public GUIElement
	{
	public:
		Event<void(float newPosition)> onScrollPositionChanged;

		void setHandleSize(UINT32 size);
		void setScrollPos(float pct);

		float getScrollPos() const;

		/**
		 * @brief	Scrolls the contents by some amount. Amount is specified in the percentage
		 * 			of the entire scrollable area. Values out of range will be clamped.
		 */
		void scroll(float amount);

		UINT32 getMaxHandleSize() const;
		UINT32 getScrollableSize() const;

		virtual Vector2I _getOptimalSize() const;
	protected:
		GUIScrollBar(bool horizontal, const String& styleName, const GUILayoutOptions& layoutOptions);
		virtual ~GUIScrollBar();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const GUIMaterialInfo& getMaterial(UINT32 renderElementIdx) const;

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

		/**
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();

		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const;
	private:
		GUILayout* mLayout;
		ImageSprite* mImageSprite;

		GUIButton* mUpBtn;
		GUIButton* mDownBtn;
		GUIScrollBarHandle* mHandleBtn;
		bool mHorizontal;

		static const UINT32 ButtonScrollAmount;

		void handleMoved(float handlePct);
		void upButtonClicked();
		void downButtonClicked();
	};
}