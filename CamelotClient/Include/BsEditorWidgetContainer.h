#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEditor
{
	class EditorWidgetContainer
	{
	public:
		EditorWidgetContainer(BS::GUIWidget* parent);
		virtual ~EditorWidgetContainer();

		void add(EditorWidget& widget);
		void remove(EditorWidget& widget);
		void move(EditorWidget& widget, CM::UINT32 newPosition);

		void setSize(CM::UINT32 width, CM::UINT32 height);
		void setPosition(CM::INT32 x, CM::INT32 y);
	private:
		GUITabbedTitleBar* mTitleBar;
		BS::GUIWidget* mParent;
		CM::INT32 mX, mY;
		CM::UINT32 mWidth, mHeight;
		CM::Vector<EditorWidget*>::type mWidgets;
		CM::INT32 mActiveWidget;

		static const CM::UINT32 TitleBarHeight;

		void setActiveWidget(CM::UINT32 idx);
	};
}