#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Type of GUI button states.
	 */
	enum class GUIButtonState
	{
		Normal = 0x01, /**< Normal state when button is not being iteracted with. */
		Hover = 0x02, /**< State when pointer is hovering over the button. */
		Active = 0x04, /**< State when button is being clicked. */
		Focused = 0x08, /**< State when button has been selected. */
		NormalOn = 0x11, /**< Normal state when button is not being iteracted with and is in "on" state. */
		HoverOn = 0x12, /**< State when pointer is hovering over the button and is in "on" state. */
		ActiveOn = 0x14, /**< State when button is being clicked and is in "on" state. */
		FocusedOn = 0x18 /**< State when button has been selected and is in "on" state. */
	};

	/**
	 * @brief	Base class for a clickable GUI button element.
	 */	
	class BS_EXPORT GUIButtonBase : public GUIElement
	{
	public:
		/**
		 * @brief	Change content displayed by the button.
		 */
		void setContent(const GUIContent& content);

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		/**
		 * @brief	Change the button "on" state. This state determines
		 *			whether the button uses normal or "on" fields specified
		 *			in the GUI style.
		 */
		void _setOn(bool on);

		/**
		 * @brief	Retrieves the button "on" state. This state determines
		 *			whether the button uses normal or "on" fields specified
		 *			in the GUI style.
		 */
		bool _isOn() const;

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const;

		/**
		 * @copydoc GUIElement::_getRenderElementDepthRange
		 */
		virtual UINT32 _getRenderElementDepthRange() const;

		/**
		 * @brief	Triggered when button is clicked.
		 */
		Event<void()> onClick;

		/**
		 * @brief	Triggered when pointer hovers over the button.
		 */
		Event<void()> onHover;

		/**
		 * @brief	Triggered when pointer that was previously hovering leaves the button.
		 */
		Event<void()> onOut;

		/**
		 * @brief	Triggered when button is clicked twice in rapid succession.
		 */
		Event<void()> onDoubleClick;
	protected:
		GUIButtonBase(const String& styleName, const GUIContent& content, const GUILayoutOptions& layoutOptions);
		virtual ~GUIButtonBase();

		/**
		 * @copydoc GUIElement::getNumRenderElements
		 */
		virtual UINT32 _getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial
		 */
		virtual const GUIMaterialInfo& _getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads
		 */
		virtual UINT32 _getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer
		 */
		virtual void _fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
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
		 * @copydoc GUIElement::mouseEvent
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev);

		/**
		 * @copydoc GUIElement::_getRenderElementDepth
		 */
		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const;

		/**
		 * @brief	Gets the text sprite descriptor used for creating/updating the internal text sprite.
		 */
		TEXT_SPRITE_DESC getTextDesc() const;

		/**
		 * @brief	Change the internal button state, changing the button look depending on set style.
		 */
		void setState(GUIButtonState state);

		/**
		 * @brief	Retrieves interal button state.
		 */
		GUIButtonState getState() const { return mActiveState; }

		/**
		 * @brief	Returns the active sprite texture, depending on the current state.
		 */
		const HSpriteTexture& getActiveTexture() const;

		/**
		 * @brief	Returns the active text color, depending on the current state.
		 */
		Color getActiveTextColor() const;
	private:
		ImageSprite* mImageSprite;
		ImageSprite* mContentImageSprite;
		TextSprite* mTextSprite;
		GUIButtonState mActiveState;
		Color mColor;

		IMAGE_SPRITE_DESC mImageDesc;
		GUIContent mContent;

		HEvent mLocStringUpdatedConn;
	};
}