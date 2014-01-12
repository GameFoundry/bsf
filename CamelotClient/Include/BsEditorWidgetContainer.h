#pragma once

#include "BsEditorPrerequisites.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class EditorWidgetContainer
	{
	public:
		EditorWidgetContainer(BS::GUIWidget* parent, CM::RenderWindow* renderWindow);
		virtual ~EditorWidgetContainer();

		void add(EditorWidget& widget);
		void remove(EditorWidget& widget);
		void insert(CM::UINT32 idx, EditorWidget& widget);

		void setSize(CM::UINT32 width, CM::UINT32 height);
		void setPosition(CM::INT32 x, CM::INT32 y);

		CM::UINT32 getNumWidgets() const { return (CM::UINT32)mWidgets.size(); }
		BS::GUIWidget& getParentWidget() const { return *mParent; }

		CM::RectI getContentBounds() const;

		void _notifyWidgetDestroyed(EditorWidget* widget);

		boost::signal<void()> onWidgetClosed;
	private:
		GUITabbedTitleBar* mTitleBar;
		BS::GUIWidget* mParent;
		CM::INT32 mX, mY;
		CM::UINT32 mWidth, mHeight;
		CM::Vector<EditorWidget*>::type mWidgets;
		CM::INT32 mActiveWidget;

		static const CM::UINT32 TitleBarHeight;

		void setActiveWidget(CM::UINT32 idx);
		void tabActivated(CM::UINT32 idx);
		void tabClosed(CM::UINT32 idx);
		void tabDraggedOff(CM::UINT32 idx);
		void tabDraggedOn(CM::UINT32 idx);

		static void tabDroppedCallback(bool wasDragProcessed);
	};
}