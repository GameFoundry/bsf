#pragma once

#include "BsPrerequisites.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class GUITabbedTitleBar
	{
	public:
		GUITabbedTitleBar(BS::GUIWidget* parent);
		virtual ~GUITabbedTitleBar();

		void setSize(CM::UINT32 width, CM::UINT32 height);
		void setPosition(CM::INT32 x, CM::INT32 y);

		void addTab(const CM::WString& name);
		void insertTab(CM::UINT32 idx, const CM::WString& name);
		void removeTab(CM::UINT32 idx);

		boost::signal<void(CM::UINT32)> onTabActivated;
		boost::signal<void(CM::UINT32)> onTabAdded;
		boost::signal<void(CM::UINT32)> onTabRemoved;
	protected:
		CM::Vector<BS::GUIWindowMover*>::type mDragDropElements;
		CM::Vector<BS::GUIToggle*>::type mTabButtons;

		BS::GUIWidget* mParentWidget;
		BS::GUIArea* mMainArea;
		BS::GUIArea* mBackgroundArea;
		BS::GUILayout* mMainLayout;
		BS::GUIButton* mMinBtn;
		BS::GUIButton* mCloseBtn;
		BS::GUIWindowMover* mLastDropElement;
	};
}