#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEditor
{
	class EditorWidget
	{
	public:
		virtual ~EditorWidget();

		const CM::WString& getName() const { return mName; }

		virtual void _initialize(BS::GUIWidget& widget, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);
		void _setArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);
		void _changeParent(BS::GUIWidget& widget);

	protected:
		EditorWidget(const CM::WString& name);

		CM::WString mName;
		BS::GUIWidget* mParentWidget;
		BS::GUIArea* mContent;
	};
}