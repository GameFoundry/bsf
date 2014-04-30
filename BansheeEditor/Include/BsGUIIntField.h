#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIIntField : public TGUIField<GUIIntField>
	{
	public:
		static const String& getGUITypeName();

		GUIIntField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& labelStyle, const String& inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel);

		INT32 getValue() const { return mValue; }
		void setValue(INT32 value);

		boost::signal<void(INT32)> onValueChanged;

	protected:
		virtual ~GUIIntField();

		void updateClippedBounds();

	protected:
		static const INT32 DRAG_SPEED;

		GUIInputBox* mInputBox;
		INT32 mValue;
		INT32 mLastDragPos;
		bool mIsDragging;
		bool mIsDragCursorSet;

		bool _hasCustomCursor(const Vector2I position, CursorType& type) const;
		virtual bool mouseEvent(const GUIMouseEvent& ev);

		void valueChanged(const WString& newValue);
		void focusGained();
		void focusLost();

		static bool intFilter(const WString& str);
	};
}