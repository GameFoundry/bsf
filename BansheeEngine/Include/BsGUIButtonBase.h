#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/** @addtogroup Implementation
	 *  @{
	 */

	/**	Base class for a clickable GUI button element. */	
	class BS_EXPORT GUIButtonBase : public GUIElement
	{
	public:
		/**	Change content displayed by the button. */
		void setContent(const GUIContent& content);

		/** @cond INTERNAL */

		/**
		 * Change the button "on" state. This state determines whether the button uses normal or "on" fields specified in 
		 * the GUI style.
		 */
		void _setOn(bool on);

		/**
		 * Retrieves the button "on" state. This state determines whether the button uses normal or "on" fields specified
		 * in the GUI style.
		 */
		bool _isOn() const;

		/**	Change the internal button state, changing the button look depending on set style. */
		void _setState(GUIElementState state);

		/** @copydoc GUIElement::_getOptimalSize */
		virtual Vector2I _getOptimalSize() const override;

		/** @copydoc GUIElement::_getRenderElementDepthRange */
		virtual UINT32 _getRenderElementDepthRange() const override;

		/** @endcond */

		/**	Triggered when button is clicked. */
		Event<void()> onClick;

		/**	Triggered when pointer hovers over the button. */
		Event<void()> onHover;

		/**	Triggered when pointer that was previously hovering leaves the button. */
		Event<void()> onOut;

		/**	Triggered when button is clicked twice in rapid succession. */
		Event<void()> onDoubleClick;
	protected:
		GUIButtonBase(const String& styleName, const GUIContent& content, const GUIDimensions& dimensions);
		virtual ~GUIButtonBase();

		/** @copydoc GUIElement::_getNumRenderElements */
		virtual UINT32 _getNumRenderElements() const override;

		/** @copydoc GUIElement::_getMaterial */
		virtual const SpriteMaterialInfo& _getMaterial(UINT32 renderElementIdx) const override;

		/** @copydoc GUIElement::_getNumQuads */
		virtual UINT32 _getNumQuads(UINT32 renderElementIdx) const override;

		/** @copydoc GUIElement::_fillBuffer */
		virtual void _fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const override;

		/** @copydoc GUIElement::updateRenderElementsInternal */
		virtual void updateRenderElementsInternal() override;

		/** @copydoc GUIElement::mouseEvent */
		virtual bool _mouseEvent(const GUIMouseEvent& ev) override;

		/** @copydoc GUIElement::_getRenderElementDepth */
		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const override;

		/** @copydoc GUIElement::_getTooltip */
		virtual WString _getTooltip() const override;

		/** Creates or destroys the content image sprite depending if there is a content image for the active state. */
		void refreshContentSprite();

		/**	Gets the text sprite descriptor used for creating/updating the internal text sprite. */
		TEXT_SPRITE_DESC getTextDesc() const;

		/**	Retrieves internal button state. */
		GUIElementState getState() const { return mActiveState; }

		/**	Returns the active sprite texture, depending on the current state. */
		const HSpriteTexture& getActiveTexture() const;

		/**	Returns the active text color, depending on the current state. */
		Color getActiveTextColor() const;
	private:
		ImageSprite* mImageSprite;
		ImageSprite* mContentImageSprite;
		TextSprite* mTextSprite;
		GUIElementState mActiveState;

		IMAGE_SPRITE_DESC mImageDesc;
		GUIContent mContent;
	};

	/** @} */
}