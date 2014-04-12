#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIIntField : public TGUIField<GUIIntField>
	{
	public:
		static const CM::String& getGUITypeName();

		GUIIntField(const PrivatelyConstruct& dummy, const BS::GUIContent& labelContent, CM::UINT32 labelWidth,
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* inputBoxStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);

		CM::INT32 getValue() const;
		void setValue(CM::INT32 value);

	protected:
		virtual ~GUIIntField();

		void updateClippedBounds();

	protected:
		static const CM::INT32 DRAG_SPEED;

		BS::GUIInputBox* mInputBox;
		CM::INT32 mLastDragPos;
		bool mIsDragging;
		bool mIsDragCursorSet;

		bool _hasCustomCursor(const CM::Vector2I position, BS::CursorType& type) const;
		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);

		static bool intFilter(const CM::WString& str);
	};
}