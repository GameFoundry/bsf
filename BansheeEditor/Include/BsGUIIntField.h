#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIIntField : public TGUIField<GUIIntField>
	{
	public:
		static const String& getGUITypeName();
		static const String& getInputStyleType();

		GUIIntField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& dimensions, bool withLabel);

		INT32 getValue() const { return mValue; }
		void setValue(INT32 value);
		void setRange(INT32 min, INT32 max);

		bool hasInputFocus() const { return mHasInputFocus; }

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		Event<void(INT32)> onValueChanged;
	protected:
		virtual ~GUIIntField();

		void updateClippedBounds();

		bool _hasCustomCursor(const Vector2I position, CursorType& type) const;
		virtual bool mouseEvent(const GUIMouseEvent& ev);
		void styleUpdated();

		void valueChanged(const WString& newValue);
		void focusGained();
		void focusLost();

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