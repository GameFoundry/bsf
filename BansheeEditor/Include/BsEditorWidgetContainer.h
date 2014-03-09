#pragma once

#include "BsEditorPrerequisites.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class BS_ED_EXPORT EditorWidgetContainer
	{
	public:
		EditorWidgetContainer(BS::GUIWidget* parent, CM::RenderWindow* renderWindow, EditorWindow* parentEditorWindow);
		virtual ~EditorWidgetContainer();

		void add(EditorWidgetBase& widget);
		void remove(EditorWidgetBase& widget);
		void insert(CM::UINT32 idx, EditorWidgetBase& widget);
		bool contains(EditorWidgetBase& widget);

		void setSize(CM::UINT32 width, CM::UINT32 height);
		void setPosition(CM::INT32 x, CM::INT32 y);

		CM::UINT32 getNumWidgets() const { return (CM::UINT32)mWidgets.size(); }
		EditorWidgetBase* getWidget(CM::UINT32 idx) const;
		BS::GUIWidget& getParentWidget() const { return *mParent; }
		EditorWindow* getParentWindow() const { return mParentWindow; }

		CM::RectI getContentBounds() const;

		void _notifyWidgetDestroyed(EditorWidgetBase* widget);

		boost::signal<void()> onWidgetClosed;
	private:
		EditorWindow* mParentWindow;
		GUITabbedTitleBar* mTitleBar;
		BS::GUIArea* mTitleBarArea;
		BS::GUIWidget* mParent;
		CM::INT32 mX, mY;
		CM::UINT32 mWidth, mHeight;
		CM::UnorderedMap<CM::UINT32, EditorWidgetBase*>::type mWidgets;
		CM::INT32 mActiveWidget;

		static const CM::UINT32 TitleBarHeight;

		void removeInternal(EditorWidgetBase& widget);

		void setActiveWidget(CM::UINT32 idx);
		void tabActivated(CM::UINT32 idx);
		void tabClosed(CM::UINT32 idx);
		void tabDraggedOff(CM::UINT32 idx);
		void tabDraggedOn(CM::UINT32 idx);

		static void tabDroppedCallback(bool wasDragProcessed);
	};
}