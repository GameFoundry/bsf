#pragma once

#include "BsPrerequisites.h"
#include "BsGUIButtonBase.h"
#include "BsGUIToggleGroup.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI element representing a toggle (on/off) button.
	 */
	class BS_EXPORT GUIToggle : public GUIButtonBase
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new toggle button with the specified label.
		 *
		 * @param	text			Label to display in the button, if any.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIToggle* create(const HString& text, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new toggle button with the specified label.
		 *
		 * @param	text			Label to display in the button, if any.
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIToggle* create(const HString& text, const GUIOptions& layoutOptions, 
			const String& styleName = StringUtil::BLANK);
	
		/**
		 * @brief	Creates a new toggle button with the specified label.
		 *
		 * @param	text			Label to display in the button, if any.
		 * @param	toggleGroup		Toggle group this button belongs to.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIToggle* create(const HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, 
			const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new toggle button with the specified label.
		 *
		 * @param	text			Label to display in the button, if any.
		 * @param	toggleGroup		Toggle group this button belongs to.
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIToggle* create(const HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, 
			const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		/**
		 * @copydoc	GUIToggle::create(const HString& text, const String& styleName)
		 */
		static GUIToggle* create(const GUIContent& content, const String& styleName = StringUtil::BLANK);

		/**
		 * @copydoc	GUIToggle::create(const HString& text, const GUIOptions& layoutOptions, const String& styleName)
		 */
		static GUIToggle* create(const GUIContent& content, const GUIOptions& layoutOptions, 
			const String& styleName = StringUtil::BLANK);

		/**
		 * @copydoc	GUIToggle::create(const HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, const String& styleName)
		 */
		static GUIToggle* create(const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, 
			const String& styleName = StringUtil::BLANK);

		/**
		 * @copydoc	GUIToggle::create(const HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, const GUIOptions& layoutOptions, const String& styleName)
		 */
		static GUIToggle* create(const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, 
			const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a toggle group that you may provide to GUIToggle upon construction. Toggles sharing the
		 *			same group will only have a single element active at a time.
		 */
		static std::shared_ptr<GUIToggleGroup> createToggleGroup();

		/**
		 * @brief	Checks the toggle, making it active.
		 */
		void toggleOn();

		/**
		 * @brief	Unchecks the toggle, making it inactive.
		 */
		void toggleOff();

		/**
		 * @brief	Checks is the toggle currently on.
		 */
		bool isToggled() const { return mIsToggled; }

		/**
		 * @copydoc	GUIButtonBase::getElementType
		 */
		virtual ElementType _getElementType() const { return ElementType::Toggle; }

		/**
		 * @brief	Sets a toggle group of the toggle button. Toggling one button in a group will
		 *			automatically untoggle others.
		 */
		void _setToggleGroup(std::shared_ptr<GUIToggleGroup> toggleGroup);

		/**
		 * @brief	Triggered whenever the button is toggled on or off.
		 */
		Event<void(bool)> onToggled;
	protected:
		virtual ~GUIToggle();

	protected:
		GUIToggle(const String& styleName, const GUIContent& content, 
			std::shared_ptr<GUIToggleGroup> toggleGroup, const GUIDimensions& layoutOptions);

		/**
		 * @copydoc	GUIButtonBase::mouseEvent
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev);

	protected:
		std::shared_ptr<GUIToggleGroup> mToggleGroup;
		bool mIsToggled;
	};
}