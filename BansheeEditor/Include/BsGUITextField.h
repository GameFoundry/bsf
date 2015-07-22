#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	/**
	 * @brief	A composite GUI object representing an editor field. Editor fields are a combination
	 *			of a label and an input field. Label is optional. This specific implementation
	 *			displays a text input field.
	 */
	class BS_ED_EXPORT GUITextField : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * Returns style type name of the internal GUILabel element.
		 */
		static const String& getLabelStyleType();

		/**
		 * Returns style type name of the internal GUIInputBox element.
		 */
		static const String& getInputStyleType();

		/**
		 * @brief	Creates a new GUI text editor field with a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI text editor field with a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const GUIContent& labelContent, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI text editor field with a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	labelText		String to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const HString& labelText, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI text editor field with a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	labelText		String to display in the editor field label.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const HString& labelText, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI text editor field without a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const GUIOptions& options, const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI text editor field with a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI text editor field with a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const GUIContent& labelContent,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI text editor field with a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	labelText		String to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const HString& labelText, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI text editor field with a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	labelText		String to display in the editor field label.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const HString& labelText,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI text editor field without a label.
		 *
		 * @param	multiline		Should the input box accept multiple lines of input.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextField* create(bool multiline, const String& style = StringUtil::BLANK);

		GUITextField(const PrivatelyConstruct& dummy, bool multiline, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& dimensions, bool withLabel);

		/**
		 * @brief	Returns the value currently input in the text field.
		 */
		WString getValue() const { return mValue; }

		/**
		 * @brief	Sets a new value in the text field.
		 */
		void setValue(const WString& value);

		/**
		 * @brief	Checks whether the element currently has keyboard focus.
		 */
		bool hasInputFocus() const { return mHasInputFocus; }

		Event<void(const WString&)> onValueChanged; /** Triggered when the value in the field changes. */
	protected:
		static const UINT32 DEFAULT_LABEL_WIDTH;

		virtual ~GUITextField();

		/**
		 * @copydoc	GUIElement::styleUpdated
		 */
		void styleUpdated() override;

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color) override;

		/**
		 * @copydoc	GUIElement::_updateLayoutInternal
		 */
		void _updateLayoutInternal(const GUILayoutData& data) override;

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		Vector2I _getOptimalSize() const override;

		/**
		 * @brief	Triggered when the value in the internal input box changes.
		 */
		void valueChanged(const WString& newValue);

		/**
		 * @brief	Triggered when the internal input box gains focus.
		 */
		void focusGained();

		/**
		 * @brief	Triggered when the internal input box loses focus.
		 */
		void focusLost();

		GUIInputBox* mInputBox;
		GUILayout* mLayout;
		GUILabel* mLabel;
		bool mHasInputFocus;
		WString mValue;
	};
}