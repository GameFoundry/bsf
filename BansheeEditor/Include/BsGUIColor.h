#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsGUIContent.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI element that displays the set color. RGB and alpha
	 *			values are displayed separately.
	 */
	class GUIColor : public GUIElement
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new GUI color element.
		 *
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIColor* create(const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI color element.
		 *
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIColor* create(const GUIOptions& options, const String& styleName = StringUtil::BLANK);

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const override;

		/**
		 * @brief	Sets the color to display.
		 */
		void setColor(const Color& color);

		/**
		 * @brief	Returns the currently displayed color.
		 */
		Color getColor() const { return mValue; }

		Event<void()> onClicked; /**< Triggered when the user clicks on the GUI element. */
	protected:
		GUIColor(const String& styleName, const GUIDimensions& dimensions);
		virtual ~GUIColor();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual UINT32 _getNumRenderElements() const override;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const GUIMaterialInfo& _getMaterial(UINT32 renderElementIdx) const override;

		/**
		 * @copydoc GUIElement::getNumQuads()
		 */
		virtual UINT32 _getNumQuads(UINT32 renderElementIdx) const override;

		/**
		 * @copydoc GUIElement::fillBuffer()
		 */
		virtual void _fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const override;

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal() override;

		/**
		 * @copydoc GUIElement::_mouseEvent()
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev) override;

	private:
		static const float ALPHA_SPLIT_POSITION;

		ImageSprite* mColorSprite;
		ImageSprite* mAlphaSprite;

		IMAGE_SPRITE_DESC mColorImageDesc;
		IMAGE_SPRITE_DESC mAlphaImageDesc;

		Color mValue;
	};
}