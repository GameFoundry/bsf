//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
		 */
		void setHandleSize(UINT32 size);

		/**
		 * @brief	Sets the position of the scroll handle in percent (ranging [0, 1]).
		 */
		void setScrollPos(float pct);

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
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const;
	protected:
		/**
		 * @brief	Constructs a new scrollbar.
		 *
		 * @param	horizontal	If true the scroll bar will have a horizontal moving handle, otherwise
		 *						it will be a vertical one.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 */
		GUIScrollBar(bool horizontal, const String& styleName, const GUILayoutOptions& layoutOptions);
		virtual ~GUIScrollBar();

		/**
		 * @copydoc GUIElement::getNumRenderElements
		 */
		virtual UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial
		 */
		virtual const GUIMaterialInfo& getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads
		 */
		virtual UINT32 getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer
		 */
		virtual void fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal
		 */
		virtual void updateRenderElementsInternal();

		/**
		 * @copydoc GUIElement::updateBounds
		 */
		virtual void updateClippedBounds();

		/**
		 * @copydoc GUIElement::_getRenderElementDepth
		 */
		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const;
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
		GUIScrollBarHandle* mHandleBtn;
		bool mHorizontal;

		static const UINT32 ButtonScrollAmount;
	};
}