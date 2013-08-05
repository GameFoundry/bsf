#pragma once

#include "BsPrerequisites.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class GUITabbedTitleBar
	{
		struct Tab
		{
			Tab(BS::GUIToggle* _toggle, CM::UINT32 _index)
				:toggle(_toggle), index(_index)
			{ }

			BS::GUIToggle* toggle;
			CM::UINT32 index;
		};

	public:
		GUITabbedTitleBar(BS::GUIWidget* parent);
		virtual ~GUITabbedTitleBar();

		void setSize(CM::UINT32 width, CM::UINT32 height);
		void setPosition(CM::INT32 x, CM::INT32 y);

		void addTab(const CM::WString& name);
		void insertTab(CM::UINT32 idx, const CM::WString& name);
		void removeTab(CM::UINT32 idx);

		boost::signal<void(CM::UINT32)> onTabActivated;
		boost::signal<void(CM::UINT32)> onTabClosed;
	protected:
		CM::Vector<BS::GUIWindowMover*>::type mDragDropElements;
		CM::Vector<Tab>::type mTabButtons;

		CM::UINT32 mUniqueTabIdx;
		CM::UINT32 mActiveTabIdx;
		BS::GUIWidget* mParentWidget;
		BS::GUIArea* mMainArea;
		BS::GUIArea* mBackgroundArea;
		BS::GUILayout* mMainLayout;
		BS::GUIButton* mMinBtn;
		BS::GUIButton* mCloseBtn;
		BS::GUIWindowMover* mLastDropElement;

		void tabToggled(CM::UINT32 tabIdx);
		void tabClosed();

		CM::INT32 uniqueIdxToIdx(CM::UINT32 uniqueIdx) const;
	};
}