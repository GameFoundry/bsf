#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI element that serves as a title for a Component object. The foldout
	 *			displays a certain label (e.g. component name) and can be expanded or collapsed.
	 *
	 * @note	It's up to the caller to actually hook up what expanding or collapsing the foldout means,
	 *			internally it just toggles a button.
	 */
	class BS_ED_EXPORT GUIComponentFoldout : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * Returns the style type name of the internal toggle button.
		 */
		static const String& getFoldoutButtonStyleType();

		/**
		 * @brief	Creates a new GUI component foldout element.
		 *
		 * @param	label			Label to display in the foldout title.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIComponentFoldout* create(const HString& label, const GUIOptions& options, const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI component foldout element.
		 *
		 * @param	label			Label to display in the foldout title.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIComponentFoldout* create(const HString& label, const String& style = StringUtil::BLANK);

		GUIComponentFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style, const GUIDimensions& dimensions);

		/**
		 * @brief	Returns whether the foldout is currently expanded or collapsed.
		 */
		bool isExpanded() const { return mIsExpanded; }

		/**
		 * @brief	Expands or collapses the foldout.
		 */
		void setExpanded(bool expanded);

		/**
		 * Changes the label displayed on the foldout.
		 */
		void setContent(const GUIContent& content);

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

		Event<void(bool)> onStateChanged;
	protected:
		virtual ~GUIComponentFoldout();

		/**
		 * @brief	Triggered when the foldout is expanded or collapsed.
		 *			True means expanded, false collapsed.
		 */
		void toggleTriggered(bool value);

		/**
		 * @copydoc	GUIElement::styleUpdated
		 */
		void styleUpdated() override;

		GUIToggle* mToggle;
		bool mIsExpanded;
	};
}