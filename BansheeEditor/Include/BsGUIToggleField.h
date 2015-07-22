#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	/**
	 * @brief	A composite GUI object representing an editor field. Editor fields are a combination
	 *			of a label and an input field. Label is optional. This specific implementation
	 *			displays a toggle button.
	 */
	class BS_ED_EXPORT GUIToggleField : public TGUIField<GUIToggleField>
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * Returns style type name of the internal GUIToggle element.
		 */
		static const String& getToggleStyleType();

		GUIToggleField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& dimensions, bool withLabel);

		/**
		 * @brief	Returns the current value of the toggle.
		 */
		bool getValue() const { return mValue; }

		/**
		 * @brief	Activates/deactivates the toggle.
		 */
		void setValue(bool value);

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color) override;

		Event<void(bool)> onValueChanged; /** Triggered when the value of the toggle changes. */
	protected:
		virtual ~GUIToggleField() { }

		/**
		 * @copydoc	GUIElement::styleUpdated
		 */
		void styleUpdated() override;

		/**
		 * @brief	Triggered when the value of the internal toggle button changes.
		 */
		void valueChanged(bool newValue);

		GUIToggle* mToggle;
		bool mValue;
	};
}