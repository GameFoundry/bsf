#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	/**
	 * @brief	A composite GUI object representing an editor field. Editor fields are a combination
	 *			of a label and an input field. Label is optional. This specific implementation
	 *			displays a floating point input field.
	 */
	class BS_ED_EXPORT GUIFloatField : public TGUIField<GUIFloatField>
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * Style type name for the internal input box.
		 */
		static const String& getInputStyleType();

		GUIFloatField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& dimensions, bool withLabel);

		/**
		 * @brief	Returns the value of the input field.
		 */
		float getValue() const { return mValue; }

		/**
		 * @brief	Sets a new value in the input field.
		 */
		void setValue(float value);

		/**
		 * @brief	Sets a minimum and maximum allow values in the input field.
		 *			Set to large negative/positive values if you don't require clamping.
		 */
		void setRange(float min, float max);

		/**
		 * @brief	Checks is the input field currently active.
		 */
		bool hasInputFocus() const { return mHasInputFocus; }

		/**
		 * @copydoc	GUIElement::setTint
		 */
		void setTint(const Color& color) override;

		/**
		 * @brief	Sets a new value in the input field, and also allows you to choose should the field trigger an
		 *			onValueChanged event.
		 */
		void _setValue(float value, bool triggerEvent);

		Event<void(float)> onValueChanged; /**< Triggers when the field value changes. */
		Event<void()> onConfirm; /**< Triggered when the user hits the Enter key with the input box in focus. */
	protected:
		virtual ~GUIFloatField();

		/**
		 * @copydoc	GUIElementContainer::_hasCustomCursor
		 */
		bool _hasCustomCursor(const Vector2I position, CursorType& type) const override;

		/**
		 * @copydoc	GUIElementContainer::_mouseEvent
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev) override;

		/**
		 * @copydoc	GUIElementContainer::styleUpdated
		 */
		void styleUpdated() override;

		/**
		 * @brief	Triggered when the input box value changes.
		 */
		void valueChanged(const WString& newValue);

		/**
		 * @brief	Triggered when the input box value changes, but unlike the previous
		 *			overload the value is parsed into a floating point value.
		 */
		void valueChanged(float newValue);

		/**
		 * @brief	Triggers when the input box receives or loses keyboard focus.
		 */
		void focusChanged(bool focus);

		/**
		 * @brief	Triggered when the users confirms input in the input box.
		 */
		void inputConfirmed();

		/**
		 * @brief	Callback that checks can the provided string be
		 *			converted to a floating point value.
		 */
		static bool floatFilter(const WString& str);

		static const float DRAG_SPEED;

		GUIInputBox* mInputBox;
		float mValue;
		INT32 mLastDragPos;
		float mMinValue;
		float mMaxValue;
		bool mIsDragging;
		bool mHasInputFocus;
	};
}