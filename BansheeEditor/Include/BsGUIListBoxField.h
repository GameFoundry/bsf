#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	/**
	 * @brief	A composite GUI object representing an editor field. Editor fields are a combination
	 *			of a label and an input field. Label is optional. This specific implementation
	 *			displays a list box field.
	 */
	class BS_ED_EXPORT GUIListBoxField : public GUIElementContainer
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
		 * Style type name for the internal list box.
		 */
		static const String& getListBoxStyleType();

		/**
		 * @brief	Creates a new GUI list box field with a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const GUIContent& labelContent, UINT32 labelWidth, 
			const GUIOptions& options, const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI list box field with a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const GUIContent& labelContent, 
			const GUIOptions& options, const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI list box field with a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	labelText		String to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const HString& labelText, UINT32 labelWidth, 
			const GUIOptions& options, const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI list box field with a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	labelText		String to display in the editor field label.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const HString& labelText, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI list box field without a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const GUIOptions& options, 
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI list box field with a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI list box field with a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const GUIContent& labelContent,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI list box field with a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	labelText		String to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const HString& labelText, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI list box field with a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	labelText		String to display in the editor field label.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const HString& labelText,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI list box field without a label.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBoxField* create(const Vector<HString>& elements, const String& style = StringUtil::BLANK);

		GUIListBoxField(const PrivatelyConstruct& dummy, const Vector<HString>& elements, const GUIContent& labelContent, 
			UINT32 labelWidth, const String& style, const GUIDimensions& dimensions, bool withLabel);

		/**
		 * @brief	Changes the list box elements.
		 */
		void setElements(const Vector<HString>& elements);

		/**
		 * @brief	Returns the index of the currently selected element.
		 */
		UINT32 getIndex() const { return mIndex; }

		/**
		 * @brief	Selects an element with the specified index.
		 */
		void setIndex(UINT32 value);

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color) override;

		Event<void(UINT32)> onSelectionChanged; /**< Triggers when a new element is selected. Provides index to the element. */
	protected:
		static const UINT32 DEFAULT_LABEL_WIDTH;

		virtual ~GUIListBoxField();

		/**
		 * @copydoc	GUIElement::_updateLayoutInternal
		 */
		void _updateLayoutInternal(const GUILayoutData& data) override;

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		Vector2I _getOptimalSize() const override;

		/**
		 * @copydoc	GUIElement::styleUpdated
		 */
		void styleUpdated() override;

		/**
		 * @brief	Triggered when the selected list box element changes.
		 */
		void selectionChanged(UINT32 newIndex);

		GUIListBox* mListBox;
		GUILayout* mLayout;
		GUILabel* mLabel;
		UINT32 mIndex;
	};
}