#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	/**
	 * @brief	A composite GUI object representing an editor field. Editor fields are a combination
	 *			of a label and an input field. Label is optional. This specific implementation
	 *			displays a integer input field.
	 */
	class BS_ED_EXPORT GUIIntField : public TGUIField<GUIIntField>
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

		GUIIntField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& dimensions, bool withLabel);

		/**
		 * @brief	Returns the value of the input field.
		 */
		INT32 getValue() const { return mValue; }

		/**
		 * @brief	Sets a new value in the input field.
		 */
		void setValue(INT32 value);

		/**
		 * @brief	Sets a minimum and maximum allow values in the input field.
		 *			Set to large negative/positive values if you don't require clamping.
		 */
		void setRange(INT32 min, INT32 max);

		/**
		 * @brief	Checks is the input field currently active.
		 */
		bool hasInputFocus() const { return mHasInputFocus; }

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color) override;

		Event<void(INT32)> onValueChanged; /**< Triggers when the internal value changes. */
	protected:
		virtual ~GUIIntField();

		/**
		 * @copydoc	GUIElement::updateClippedBounds
		 */
		void updateClippedBounds() override;

		/**
		 * @copydoc	GUIElement::_hasCustomCursor
		 */
		bool _hasCustomCursor(const Vector2I position, CursorType& type) const override;

		/**
		 * @copydoc	GUIElement::_mouseEvent
		 */
		bool _mouseEvent(const GUIMouseEvent& ev)  override;

		/**
		 * @copydoc	GUIElement::styleUpdated
		 */
		void styleUpdated() override;

		/**
		 * @brief	Triggered when the input box value changes.
		 */
		void valueChanged(const WString& newValue);

		/**
		 * @brief	Triggered when the input box value changes, but unlike the previous
		 *			overload the value is parsed into an integer value.
		 */
		void valueChanged(INT32 newValue);

		/**
		 * @brief	Triggers when the input box receives keyboard focus.
		 */
		void focusGained();

		/**
		 * @brief	Triggers when the input box loses keyboard focus.
		 */
		void focusLost();

		/**
		 * @brief	Callback that checks can the provided string be
		 *			converted to an integer value.
		 */
		static bool intFilter(const WString& str);

		static const INT32 DRAG_SPEED;

		GUIInputBox* mInputBox;
		INT32 mValue;
		INT32 mLastDragPos;
		INT32 mMinValue;
		INT32 mMaxValue;
		bool mIsDragging;
		bool mIsDragCursorSet;
		bool mHasInputFocus;
	};
}