#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIFloatField : public TGUIField<GUIFloatField>
	{
	public:
		static const String& getGUITypeName();

		GUIFloatField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& labelStyle, const String& inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel);

		float getValue() const;
		void setValue(float value);

	protected:
		virtual ~GUIFloatField();

		void updateClippedBounds();

	protected:
		static const float DRAG_SPEED;

		GUIInputBox* mInputBox;
		INT32 mLastDragPos;
		bool mIsDragging;

		bool _hasCustomCursor(const Vector2I position, CursorType& type) const;
		virtual bool mouseEvent(const GUIMouseEvent& ev);

		static bool floatFilter(const WString& str);
	};
}