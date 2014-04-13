#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "CmRectI.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class GUITabbedTitleBar : public BS::GUIElementContainer
	{
	public:
		static const CM::String& getGUITypeName();

		static GUITabbedTitleBar* create(CM::RenderWindow* parentWindow,
			const CM::String& backgroundStyle = CM::StringUtil::BLANK, const CM::String& tabBtnStyle = CM::StringUtil::BLANK, 
			const CM::String& minBtnStyle = CM::StringUtil::BLANK, const CM::String& closeBtnStyle = CM::StringUtil::BLANK);

		static GUITabbedTitleBar* create(CM::RenderWindow* parentWindow, const BS::GUILayoutOptions& layoutOptions, 
			const CM::String& backgroundStyle = CM::StringUtil::BLANK, const CM::String& tabBtnStyle = CM::StringUtil::BLANK, 
			const CM::String& minBtnStyle = CM::StringUtil::BLANK, const CM::String& closeBtnStyle = CM::StringUtil::BLANK);

		void addTab(const CM::HString& name);
		CM::UINT32 insertTab(CM::UINT32 position, const CM::HString& name);
		void removeTab(CM::UINT32 uniqueIdx);
		void setActive(CM::UINT32 uniqueIdx);
		CM::UINT32 getTabIdx(CM::UINT32 position) const;
		CM::UINT32 getNumTabs() const { return (CM::UINT32)mTabButtons.size(); }

		CM::Vector<CM::RectI>::type calcDraggableAreas(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height) const;

		boost::signal<void(CM::UINT32)> onTabActivated;
		boost::signal<void(CM::UINT32)> onTabClosed;
		boost::signal<void(CM::UINT32)> onTabDraggedOff;
		boost::signal<void(CM::UINT32)> onTabDraggedOn;

	protected:
		virtual ~GUITabbedTitleBar();

		void updateClippedBounds();

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);
	protected:
		static const CM::UINT32 TAB_SPACING;
		static const CM::UINT32 OPTION_BTN_SPACING;

		CM::Vector<GUITabButton*>::type mTabButtons;

		CM::UINT32 mUniqueTabIdx;
		CM::UINT32 mActiveTabIdx;
		BS::GUITexture* mBackgroundImage;
		BS::GUIButton* mMinBtn;
		BS::GUIButton* mCloseBtn;
		BS::GUIToggleGroupPtr mTabToggleGroup;
		CM::RenderWindow* mParentWindow;

		EditorWidgetBase* mTempDraggedWidget;
		CM::UINT32 mTempDraggedTabIdx;

		bool mDragInProgress;
		GUITabButton* mDraggedBtn;
		CM::INT32 mDragBtnOffset;
		CM::INT32 mInitialDragOffset;

		CM::String mBackgroundStyle;
		CM::String mCloseBtnStyle;
		CM::String mMinimizeBtnStyle;
		CM::String mTabBtnStyle;

		GUITabbedTitleBar(CM::RenderWindow* parentWindow, const CM::String& backgroundStyle, const CM::String& tabBtnStyle, 
			const CM::String& minBtnStyle, const CM::String& closeBtnStyle, const BS::GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);

		void startDrag(CM::UINT32 seqIdx, const CM::Vector2I& startDragPos);
		void endDrag();

		void tabToggled(CM::UINT32 tabIdx, bool toggledOn);
		void tabClosed();
		void tabDragged(CM::UINT32 tabIdx, const CM::Vector2I& dragPos);
		void tabDragEnd(CM::UINT32 tabIdx, const CM::Vector2I& dragPos);

		CM::INT32 uniqueIdxToSeqIdx(CM::UINT32 uniqueIdx) const;
	};
}