#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIFloatField : public TGUIField<GUIFloatField>
	{
	public:
		static const String& getGUITypeName();
		static const String& getInputStyleType();

		GUIFloatField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& dimensions, bool withLabel);

		float getValue() const { return mValue; }
		void setValue(float value);

		bool hasInputFocus() const { return mHasInputFocus; }

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		Event<void(float)> onValueChanged;
	protected:
		virtual ~GUIFloatField();

		void updateClippedBounds();

		bool _hasCustomCursor(const Vector2I position, CursorType& type) const;
		virtual bool mouseEvent(const GUIMouseEvent& ev);
		void styleUpdated();

		void valueChanged(const WString& newValue);
		void focusGained();
		void focusLost();

		static bool floatFilter(const WString& str);

		static const float DRAG_SPEED;

		GUIInputBox* mInputBox;
		float mValue;
		INT32 mLastDragPos;
		bool mIsDragging;
		bool mHasInputFocus;
	};
}