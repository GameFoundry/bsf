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
		 * @brief	Change the internal button state, changing the button look depending on set style.
		 */
		void _setState(GUIElementState state);

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const override;

		/**
		 * @copydoc GUIElement::_getRenderElementDepthRange
		 */
		virtual UINT32 _getRenderElementDepthRange() const override;

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
		GUIButtonBase(const String& styleName, const GUIContent& content, const GUIDimensions& dimensions);
		virtual ~GUIButtonBase();

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
		 * @copydoc GUIElement::mouseEvent
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev) override;

		/**
		 * @copydoc GUIElement::_getRenderElementDepth
		 */
		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const override;

		/**
		 * @brief	Creates or destroys the content image sprite depending if there is a content image for the
		 *			active state.
		 */
		void refreshContentSprite();

		/**
		 * @brief	Gets the text sprite descriptor used for creating/updating the internal text sprite.
		 */
		TEXT_SPRITE_DESC getTextDesc() const;

		/**
		 * @brief	Retrieves internal button state.
		 */
		GUIElementState getState() const { return mActiveState; }

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
		GUIElementState mActiveState;

		IMAGE_SPRITE_DESC mImageDesc;
		GUIContent mContent;
	};
}