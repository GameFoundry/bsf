//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsRect2I.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Tabbed title bar to be used in editor windows. Displays tabs that can
	 *			be activated, reordered by dragging, or dragged off and on to/on other title bars.
	 */
	class GUITabbedTitleBar : public GUIElementContainer
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new GUI foldout element.
		 *
		 * @param	backgroundStyle		GUI style to display the background in. Keep as blank for default.
		 * @param	backgroundStyle		GUI style to display the tab buttons in. Keep as blank for default.
		 * @param	maxBtnStyle			GUI style to display the maximize button in. Keep as blank for default.
		 * @param	closeBtnStyle		GUI style to display the close button in. Keep as blank for default.
		 */
		static GUITabbedTitleBar* create(const String& backgroundStyle = StringUtil::BLANK, const String& tabBtnStyle = StringUtil::BLANK, 
			const String& maxBtnStyle = StringUtil::BLANK, const String& closeBtnStyle = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI foldout element.
		 *
		 *
		 * @param	options				Options that allow you to control how is the element positioned and sized.
		 *								This will override any similar options set by style.
		 * @param	backgroundStyle		GUI style to display the background in. Keep as blank for default.
		 * @param	backgroundStyle		GUI style to display the tab buttons in. Keep as blank for default.
		 * @param	maxBtnStyle			GUI style to display the maximize button in. Keep as blank for default.
		 * @param	closeBtnStyle		GUI style to display the close button in. Keep as blank for default.
		 */
		static GUITabbedTitleBar* create(const GUIOptions& options,
			const String& backgroundStyle = StringUtil::BLANK, const String& tabBtnStyle = StringUtil::BLANK, 
			const String& maxBtnStyle = StringUtil::BLANK, const String& closeBtnStyle = StringUtil::BLANK);

		/**
		 * @brief	Adds a new tab to the end of the tab list.
		 *
		 * @param	name	Title to display on the tab button.
		 *
		 * @returns	A unique index (not sequential) that you may use for later identifying the tab.
		 */
		UINT32 addTab(const HString& name);

		/**
		 * @brief	Inserts a new tab button at the specified position.
		 *
		 * @param	position	Sequential index to insert the tab button in. This will be clamped
		 *						to a valid range.
		 * @param	name		Title to display on the tab button.
		 *
		 * @returns	A unique index (not sequential) that you may use for later identifying the tab.
		 */
		UINT32 insertTab(UINT32 position, const HString& name);

		/**
		 * @brief	Removes the tab button with the specified unique index.
		 */
		void removeTab(UINT32 uniqueIdx);

		/**
		 * @brief	Activates the tab button with the specified unique index.
		 */
		void setActive(UINT32 uniqueIdx);

		/**
		 * @brief	Finds the unique tab index from the provided sequential tab position.
		 */
		UINT32 getTabIdx(UINT32 position) const;

		/**
		 * @brief	Returns the total number of display tab buttons.
		 */
		UINT32 getNumTabs() const { return (UINT32)mTabButtons.size(); }

		/**
		 * @brief	Changes the displayed title for a tab with the specified index.
		 */
		void updateTabName(UINT32 uniqueIdx, const HString& name);

		/**
		 * @brief	Calculates areas between the tab buttons and other GUI elements on the title bar.
		 *			These areas are normally used for setting up valid areas the user can click on and drag
		 *			the window the title bar belongs to.
		 */
		Vector<Rect2I> calcDraggableAreas(INT32 x, INT32 y, UINT32 width, UINT32 height) const;

		/**
		 * @brief	Triggered when the active tab changes. Provided parameter is the
		 *			unique index of the activated tab.
		 */
		Event<void(UINT32)> onTabActivated; 

		/**
		 * @brief	Triggered when a tab is closed. Provided parameter is the
		 *			unique index of the closed tab.
		 */
		Event<void(UINT32)> onTabClosed;

		/**
		 * @brief	Triggered when a tab maximize button is clicked. Provided parameter is the
		 *			unique index of the maximized/restored tab.
		 */
		Event<void(UINT32)> onTabMaximized;

		/**
		 * @brief	Triggered when a tab gets dragged off the title bar. Provided 
		 *			parameter is the unique index of the activated tab.
		 */
		Event<void(UINT32)> onTabDraggedOff;

		/**
		 * @brief	Triggered when a new tab gets dragged on the title bar. Provided 
		 *			parameter is the sequential index of the activated tab.
		 */
		Event<void(UINT32)> onTabDraggedOn;

	protected:
		GUITabbedTitleBar(const String& backgroundStyle, const String& tabBtnStyle, 
			const String& minBtnStyle, const String& closeBtnStyle, const GUIDimensions& dimensions);

		virtual ~GUITabbedTitleBar();

		/**
		 * @copydoc GUIElementContainer::updateClippedBounds
		 */
		void updateClippedBounds() override;

		/**
		 * @copydoc GUIElementContainer::_getOptimalSize
		 */
		Vector2I _getOptimalSize() const override;

		/**
		 * @copydoc GUIElementContainer::_updateLayoutInternal
		 */
		void _updateLayoutInternal(const GUILayoutData& data) override;

		/**
		 * @copydoc GUIElementContainer::_mouseEvent
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev) override;

		/**
		 * @brief	Starts the internal drag and drop operation.
		 *
		 * @param	seqIdx			Sequential index of the dragged tab.
		 * @param	startDragPos	Pointer position of where the drag originated, relative
		 *							to parent widget.
		 */
		void startDrag(UINT32 seqIdx, const Vector2I& startDragPos);

		/**
		 * @brief	Ends the internal drag and drop operation started with ::startDrag.
		 */
		void endDrag();

		/**
		 * @brief	Triggered when a tab button is toggled on or off.
		 *
		 * @param	tabIdx		Unique index of the tab.
		 * @param	toggledOn	Whether the tab was activated or deactivated.
		 */
		void tabToggled(UINT32 tabIdx, bool toggledOn);

		/**
		 * @brief	Triggered when the close button is pressed.
		 */
		void tabClosed();

		/**
		 * @brief	Triggered when the maximize button is pressed.
		 */
		void tabMaximize();

		/**
		 * @brief	Triggered every frame while a tab button is being dragged.
		 *
		 * @param	tabIdx	Unique index of the dragged tab.
		 * @param	dragPos	Position of the pointer, relative to parent widget.
		 */
		void tabDragged(UINT32 tabIdx, const Vector2I& dragPos);

		/**
		 * @brief	Triggered when a drag operation on a tab button ends.
		 *
		 * @param	tabIdx	Unique index of the dragged tab.
		 * @param	dragPos	Position of the pointer, relative to parent widget.
		 */
		void tabDragEnd(UINT32 tabIdx, const Vector2I& dragPos);

		/**
		 * @brief	Converts unique tab index to a sequential index corresponding to the
		 *			tab's position in the title bar.
		 */
		INT32 uniqueIdxToSeqIdx(UINT32 uniqueIdx) const;

		static const INT32 TAB_SPACING;
		static const INT32 FIRST_TAB_OFFSET;
		static const INT32 OPTION_BTN_SPACING;
		static const INT32 OPTION_BTN_RIGHT_OFFSET;

		Vector<GUITabButton*> mTabButtons;

		UINT32 mUniqueTabIdx;
		UINT32 mActiveTabIdx;
		GUITexture* mBackgroundImage;
		GUIButton* mMaxBtn;
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
		String mMaximizeBtnStyle;
		String mTabBtnStyle;
	};
}