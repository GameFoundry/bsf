#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIFloatField : public TGUIField<GUIFloatField>
	{
	public:
		static const CM::String& getGUITypeName();

		GUIFloatField(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, const BS::GUIContent& labelContent, CM::UINT32 labelWidth,
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* inputBoxStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);

		float getValue() const;
		void setValue(float value);

	protected:
		virtual ~GUIFloatField();

		void updateClippedBounds();

	protected:
		static const float DRAG_SPEED;

		BS::GUIInputBox* mInputBox;
		CM::INT32 mLastDragPos;
		bool mIsDragging;

		bool _hasCustomCursor(const CM::Vector2I position, BS::CursorType& type) const;
		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);

		static bool floatFilter(const CM::WString& str);
	};
}