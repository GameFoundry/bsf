#pragma once

#include "BsEditorPrerequisites.h"
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
		boost::signal<void(CM::UINT32)> onTabClosed;
		boost::signal<void(CM::UINT32)> onTabDraggedOff;
		boost::signal<void(CM::UINT32)> onTabDraggedOn;
	protected:
		CM::Vector<GUIWindowMover*>::type mDragDropElements;
		CM::Vector<GUITabButton*>::type mTabButtons;

		CM::UINT32 mUniqueTabIdx;
		CM::UINT32 mActiveTabIdx;
		BS::GUIWidget* mParentWidget;
		BS::GUIArea* mMainArea;
		BS::GUIArea* mBackgroundArea;
		BS::GUILayout* mMainLayout;
		BS::GUIButton* mMinBtn;
		BS::GUIButton* mCloseBtn;
		GUIWindowMover* mLastDropElement;

		void tabToggled(CM::UINT32 tabIdx);
		void tabClosed();
		void tabDraggedOff(CM::UINT32 tabIdx);
		void tabDraggedOn(CM::UINT32 tabIdx);

		CM::INT32 uniqueIdxToSeqIdx(CM::UINT32 uniqueIdx) const;
	};
}