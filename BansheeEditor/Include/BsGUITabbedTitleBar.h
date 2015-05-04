#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsRect2I.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class GUITabbedTitleBar : public GUIElementContainer
	{
	public:
		static const String& getGUITypeName();

		static GUITabbedTitleBar* create(const String& backgroundStyle = StringUtil::BLANK, const String& tabBtnStyle = StringUtil::BLANK, 
			const String& minBtnStyle = StringUtil::BLANK, const String& closeBtnStyle = StringUtil::BLANK);

		static GUITabbedTitleBar* create(const GUIDimensions& dimensions,
			const String& backgroundStyle = StringUtil::BLANK, const String& tabBtnStyle = StringUtil::BLANK, 
			const String& minBtnStyle = StringUtil::BLANK, const String& closeBtnStyle = StringUtil::BLANK);

		void addTab(const HString& name);
		UINT32 insertTab(UINT32 position, const HString& name);
		void removeTab(UINT32 uniqueIdx);
		void setActive(UINT32 uniqueIdx);
		UINT32 getTabIdx(UINT32 position) const;
		UINT32 getNumTabs() const { return (UINT32)mTabButtons.size(); }

		Vector<Rect2I> calcDraggableAreas(INT32 x, INT32 y, UINT32 width, UINT32 height) const;

		Event<void(UINT32)> onTabActivated;
		Event<void(UINT32)> onTabClosed;
		Event<void(UINT32)> onTabDraggedOff;
		Event<void(UINT32)> onTabDraggedOn;

	protected:
		virtual ~GUITabbedTitleBar();

		void updateClippedBounds();

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			Rect2I clipRect, UINT8 widgetDepth, INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax);
	protected:
		static const UINT32 TAB_SPACING;
		static const UINT32 OPTION_BTN_SPACING;

		Vector<GUITabButton*> mTabButtons;

		UINT32 mUniqueTabIdx;
		UINT32 mActiveTabIdx;
		GUITexture* mBackgroundImage;
		GUIButton* mMinBtn;
		GUIButton* mCloseBtn;
		GUIToggleGroupPtr mTabToggleGroup;

		EditorWidgetBase* mTempDraggedWidget;
		UINT32 mTempDraggedTabIdx;

		bool mDragInProgress;
		GUITabButton* mDraggedBtn;
		INT32 mDragBtnOffset;
		INT32 mInitialDragOffset;

		String mBackgroundStyle;
		String mCloseBtnStyle;
		String mMinimizeBtnStyle;
		String mTabBtnStyle;

		GUITabbedTitleBar(const String& backgroundStyle, const String& tabBtnStyle, 
			const String& minBtnStyle, const String& closeBtnStyle, const GUIDimensions& dimensions);

		virtual bool _mouseEvent(const GUIMouseEvent& ev);

		void startDrag(UINT32 seqIdx, const Vector2I& startDragPos);
		void endDrag();

		void tabToggled(UINT32 tabIdx, bool toggledOn);
		void tabClosed();
		void tabDragged(UINT32 tabIdx, const Vector2I& dragPos);
		void tabDragEnd(UINT32 tabIdx, const Vector2I& dragPos);

		INT32 uniqueIdxToSeqIdx(UINT32 uniqueIdx) const;
	};
}