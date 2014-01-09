#pragma once

#include "BsEditorPrerequisites.h"
#include "CmRectI.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class GUITabbedTitleBar
	{
	public:
		GUITabbedTitleBar(BS::GUIWidget* parent, CM::RenderWindow* parentWindow);
		virtual ~GUITabbedTitleBar();

		void setSize(CM::UINT32 width, CM::UINT32 height);
		void setPosition(CM::INT32 x, CM::INT32 y);

		void addTab(const CM::HString& name);
		void insertTab(CM::UINT32 idx, const CM::HString& name);
		void removeTab(CM::UINT32 idx);

		boost::signal<void(CM::UINT32)> onTabActivated;
		boost::signal<void(CM::UINT32)> onTabClosed;
		boost::signal<void(CM::UINT32)> onTabDraggedOff;
		boost::signal<void(CM::UINT32)> onTabDraggedOn;
	protected:
		static const CM::UINT32 TAB_SPACING;
		static const CM::UINT32 OPTION_BTN_SPACING;

		CM::Vector<GUITabButton*>::type mTabButtons;

		CM::UINT32 mUniqueTabIdx;
		CM::UINT32 mActiveTabIdx;
		BS::GUIWidget* mParentWidget;
		BS::GUIArea* mBackgroundArea;
		BS::GUIButton* mMinBtn;
		BS::GUIButton* mCloseBtn;
		CM::RenderWindow* mParentWindow;
		CM::RectI mArea;

		bool mDragInProgress;
		GUITabButton* mDraggedBtn;
		CM::INT32 mDragBtnOffset;
		CM::INT32 mInitialDragOffset;

		const BS::GUIElementStyle* mCloseBtnStyle;
		const BS::GUIElementStyle* mMinimizeBtnStyle;

		void updateLayout(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth);

		void tabToggled(CM::UINT32 tabIdx);
		void tabClosed();
		void tabDragged(CM::UINT32 tabIdx, const CM::Vector2I& dragPos);
		void tabDragEnd(CM::UINT32 tabIdx, const CM::Vector2I& dragPos);
		void tabDraggedOn(CM::UINT32 tabIdx);

		CM::INT32 uniqueIdxToSeqIdx(CM::UINT32 uniqueIdx) const;
	};
}