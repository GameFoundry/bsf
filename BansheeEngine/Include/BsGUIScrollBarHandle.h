#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	A draggable handle used in a GUI scroll bar element.
	 */
	class BS_EXPORT GUIScrollBarHandle : public GUIElement
	{
		/**
		 * @brief	Visual state of the handle
		 */
		enum class State
		{
			Normal, Hover, Active
		};

	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new handle.
		 *
		 * @param	horizontal		Should the handle be movable vertically or horizontally.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIScrollBarHandle* create(bool horizontal, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new handle.
		 *
		 * @param	horizontal		Should the handle be movable vertically or horizontally.
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIScrollBarHandle* create(bool horizontal, const GUIOptions& layoutOptions, 
			const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Size of the handle in pixels, along the handle drag direction.
		 */
		void setHandleSize(UINT32 size);

		/**
		 * @brief	Moves the handle the the specified position in the handle area.
		 *
		 * @param	pct	Position to move the handle to, in percent ranging [0.0f, 1.0f]
		 */
		void setHandlePos(float pct);

		/**
		 * @brief	Gets the current position of the handle, in percent ranging [0.0f, 1.0f].
		 */
		float getHandlePos() const;

		/**
		 * @brief	Returns remaining length of the scrollable area not covered by the handle, in pixels.
		 */
		UINT32 getScrollableSize() const;

		/**
		 * @brief	Returns the total length of the area the handle can move in, in pixels.
		 */
		UINT32 getMaxSize() const;

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const;

		Event<void(float newPosition)> onHandleMoved;
	protected:
		~GUIScrollBarHandle();

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
	private:
		GUIScrollBarHandle(bool horizontal, const String& styleName, const GUILayoutOptions& layoutOptions);

		/**
		 * @copydoc	GUIElement::mouseEvent
		 */
		virtual bool mouseEvent(const GUIMouseEvent& ev);

		/**
		 * @brief	Checks are the specified over the scroll handle. Coordinates are relative
		 *			to the parent widget.
		 */
		bool isOnHandle(const Vector2I& pos) const;

		/**
		 * @brief	Gets the currently active texture, depending on handle state.
		 */
		const HSpriteTexture& getActiveTexture() const;

		ImageSprite* mImageSprite;
		UINT32 mHandleSize;

		bool mHorizontal; // Otherwise its vertical
		float mHandlePos;
		INT32 mDragStartPos;
		bool mMouseOverHandle;
		bool mHandleDragged;
		State mState;

	};
}