#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI element representing an element with a draggable handle
	 *			of a variable size.
	 */
	class BS_EXPORT GUIScrollBar : public GUIElement
	{
	public:
		/**
		 * @brief	Triggered whenever the scrollbar handle is moved. Value provided
		 *			is the handle position in percent (ranging [0, 1]).
		 */
		Event<void(float newPosition)> onScrollPositionChanged;

		/**
		 * @brief	Sets the size of the handle in pixels.
		 *
		 * @note	Internal method. Does not trigger layout update.
		 */
		void _setHandleSize(UINT32 size);

		/**
		 * @brief	Sets the position of the scroll handle in percent (ranging [0, 1]).
		 *
		 * @note	Internal method. Does not trigger layout update.
		 */
		void _setScrollPos(float pct);

		/**
		 * @brief	Returns the position of the scroll handle in percent (ranging [0, 1]).
		 */
		float getScrollPos() const;

		/**
		 * @brief	Moves the handle by some amount. Amount is specified in the percentage
		 * 			of the entire scrollable area. Values out of range will be clamped.
		 */
		void scroll(float amount);

		/**
		 * @brief	Returns the maximum size of the scroll handle, in pixels.
		 */
		UINT32 getMaxHandleSize() const;

		/**
		 * @brief	Returns the maximum scrollable size the handle can move within (e.g. scroll bar length).
		 */
		UINT32 getScrollableSize() const;

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color) override;

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const override;
	protected:
		/**
		 * @brief	Constructs a new scrollbar.
		 *
		 * @param	horizontal	If true the scroll bar will have a horizontal moving handle, otherwise
		 *						it will be a vertical one.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 */
		GUIScrollBar(bool horizontal, const String& styleName, const GUIDimensions& dimensions);
		virtual ~GUIScrollBar();

		/**
		 * @copydoc GUIElement::getNumRenderElements
		 */
		virtual UINT32 _getNumRenderElements() const override;

		/**
		 * @copydoc GUIElement::getMaterial
		 */
		virtual const GUIMaterialInfo& _getMaterial(UINT32 renderElementIdx) const override;

		/**
		 * @copydoc GUIElement::getNumQuads
		 */
		virtual UINT32 _getNumQuads(UINT32 renderElementIdx) const override;

		/**
		 * @copydoc GUIElement::fillBuffer
		 */
		virtual void _fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const override;

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal
		 */
		virtual void updateRenderElementsInternal() override;

		/**
		 * @copydoc GUIElement::updateBounds
		 */
		virtual void updateClippedBounds() override;

		/**
		 * @copydoc GUIElement::_getRenderElementDepth
		 */
		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const override;

		/**
		 * @copydoc	GUIElement::_getRenderElementDepthRange
		 */
		virtual UINT32 _getRenderElementDepthRange() const override;
	private:
		/**
		 * @brief	Triggered whenever the scroll handle moves. Provided value represents the new 
		 *			position of the handle in percent (ranging [0, 1]).
		 */
		void handleMoved(float handlePct);

		/**
		 * @brief	Triggered when scroll up button is clicked.
		 */
		void upButtonClicked();

		/**
		 * @brief	Triggered when scroll down button is clicked.
		 */
		void downButtonClicked();

		GUILayout* mLayout;
		ImageSprite* mImageSprite;

		GUIButton* mUpBtn;
		GUIButton* mDownBtn;
		GUISliderHandle* mHandleBtn;
		bool mHorizontal;
		Color mColor;

		static const UINT32 ButtonScrollAmount;
	};
}