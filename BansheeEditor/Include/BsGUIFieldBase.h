#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsGUIContent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for all editor GUI fields. All fields are a combination of
	 *			an optional label and an input field.
	 */
	class BS_ED_EXPORT GUIFieldBase : public GUIElementContainer
	{
	protected:
		struct PrivatelyConstruct {};

	public:
		/**
		 * @brief	Returns the style type name for the internal label element.
		 */
		static const String& getLabelStyleType()
		{
			static String LABEL_STYLE_TYPE = "EditorFieldLabel";
			return LABEL_STYLE_TYPE;
		}

		GUIFieldBase(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& labelStyle, const GUIDimensions& dimensions, bool withLabel);

		/**
		 * @copydoc	GUIElementContainer::_updateLayoutInternal
		 */
		void _updateLayoutInternal(const GUILayoutData& data) override;

		/**
		 * @copydoc	GUIElementContainer::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const override;
	protected:
		virtual ~GUIFieldBase() { }

		/**
		 * @copydoc	GUIElementContainer::styleUpdated
		 */
		virtual void styleUpdated() override;

		static const UINT32 DEFAULT_LABEL_WIDTH;

		GUILayout* mLayout;
		GUILabel* mLabel;
	};

	/**
	 * @brief	Templated GUI field class that provides common methods needed for constructing an editor field.
	 */
	template <class T>
	class TGUIField : public GUIFieldBase
	{
	public:
		/**
		 * @brief	Creates a new GUI editor field with a label.
		 *
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), labelContent, labelWidth, *curStyle,
				GUIDimensions::create(options), true);
		}

		/**
		 * @brief	Creates a new GUI editor field with a label.
		 *
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const GUIContent& labelContent, const GUIOptions& options,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
				GUIDimensions::create(options), true);
		}

		/**
		 * @brief	Creates a new GUI editor field with a label.
		 *
		 * @param	labelText		String to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const HString& labelText, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, *curStyle,
				GUIDimensions::create(options), true);
		}

		/**
		 * @brief	Creates a new GUI editor field with a label.
		 *
		 * @param	labelText		String to display in the editor field label.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const HString& labelText, const GUIOptions& options,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
				GUIDimensions::create(options), true);
		}

		/**
		 * @brief	Creates a new GUI editor field without a label.
		 *
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const GUIOptions& options, const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(), 0, *curStyle,
				GUIDimensions::create(options), false);
		}

		/**
		 * @brief	Creates a new GUI editor field with a label.
		 *
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const GUIContent& labelContent, UINT32 labelWidth,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), labelContent, labelWidth, *curStyle, GUIDimensions::create(), true);
		}

		/**
		 * @brief	Creates a new GUI editor field with a label.
		 *
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const GUIContent& labelContent,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
				GUIDimensions::create(), true);
		}

		/**
		 * @brief	Creates a new GUI editor field with a label.
		 *
		 * @param	labelText		String to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const HString& labelText, UINT32 labelWidth,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, *curStyle,
				GUIDimensions::create(), true);
		}

		/**
		 * @brief	Creates a new GUI editor field with a label.
		 *
		 * @param	labelText		String to display in the editor field label.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const HString& labelText, const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
				GUIDimensions::create(), true);
		}

		/**
		 * @brief	Creates a new GUI editor field without a label.
		 *
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static T* create(const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(), 0, *curStyle,
				GUIDimensions::create(), false);
		}

		TGUIField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& dimensions, bool withLabel)
			:GUIFieldBase(dummy, labelContent, labelWidth, style, dimensions, withLabel)
		{ }
	};
}