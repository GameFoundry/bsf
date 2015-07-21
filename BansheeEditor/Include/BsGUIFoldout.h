#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	/**
	 * @brief	Composite GUI element that contains a toggle button and a label.
	 *			Used for handling expand/collapse operations, although it's up to
	 *			the caller to actually handle the #onStateChanged event.
	 */
	class BS_ED_EXPORT GUIFoldout : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * Returns GUI style type name for the internal toggle button.
		 */
		static const String& getFoldoutButtonStyleType();

		/**
		 * Returns GUI style type name for the internal label.
		 */
		static const String& getLabelStyleType();

		/**
		 * @brief	Creates a new GUI foldout element.
		 *
		 * @param	label			Label to display in the foldout title.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIFoldout* create(const HString& label, const GUIOptions& options, const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI foldout element.
		 *
		 * @param	label			Label to display in the foldout title.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIFoldout* create(const HString& label, const String& style = StringUtil::BLANK);

		GUIFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style, const GUIDimensions& dimensions);

		/**
		 * @brief	Checks is the foldout expanded (i.e. the toggle button is active)
		 */
		bool isExpanded() const { return mIsExpanded; }

		/**
		 * @brief	Expands or collapses the foldout (i.e. changes the toggle button state)
		 */
		void setExpanded(bool expanded);

		/**
		 * Changes the label of the foldout.
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

		Event<void(bool)> onStateChanged; /**< Triggered when the foldout is expanded or collapsed. True means expanded, false otherwise. */
	protected:
		virtual ~GUIFoldout();

		/**
		 * @brief	Callback triggered when the internal toggle button is toggled.
		 */
		void toggleTriggered(bool value);

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		void styleUpdated() override;

		GUILabel* mLabel;
		GUIToggle* mToggle;

		bool mIsExpanded;
	};
}