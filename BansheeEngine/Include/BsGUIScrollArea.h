#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	/**
	 * @brief	Scroll bar options for a GUI scroll area.
	 */
	enum class ScrollBarType
	{
		ShowIfDoesntFit,
		AlwaysShow,
		NeverShow
	};

	/**
	 * @brief	A GUI element container with support for vertical & horizontal scrolling.
	 */
	class BS_EXPORT GUIScrollArea : public GUIElementContainer
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new empty scroll area.
		 *
		 * @param	vertBarType		Vertical scrollbar options.
		 * @param	horzBarType		Horizontal scrollbar options.
		 * @param	scrollBarStyle	(Optional) Style used by the scroll bars.
		 * @param	scrollAreaStyle	(Optional) Style used by the scroll content area.
		 */
		static GUIScrollArea* create(ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const String& scrollBarStyle = StringUtil::BLANK, const String& scrollAreaStyle = StringUtil::BLANK);

		/**
		 * @brief	Creates a new empty scroll area.
		 *
		 * @param	vertBarType		Vertical scrollbar options.
		 * @param	horzBarType		Horizontal scrollbar options.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	scrollBarStyle	(Optional) Style used by the scroll bars.
		 * @param	scrollAreaStyle	(Optional) Style used by the scroll content area.
		 */
		static GUIScrollArea* create(ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const GUIOptions& options, const String& scrollBarStyle = StringUtil::BLANK, 
			const String& scrollAreaStyle = StringUtil::BLANK);

		/**
		 * @brief	Creates a new empty scroll area. Scroll bars will be show if needed and hidden otherwise.
		 *
		 * @param	scrollBarStyle	(Optional) Style used by the scroll bars.
		 * @param	scrollAreaStyle	(Optional) Style used by the scroll content area.
		 */
		static GUIScrollArea* create(const String& scrollBarStyle = StringUtil::BLANK, 
			const String& scrollAreaStyle = StringUtil::BLANK);

		/**
		 * @brief	Creates a new empty scroll area. Scroll bars will be show if needed and hidden otherwise.
		 *
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	scrollBarStyle	(Optional) Style used by the scroll bars.
		 * @param	scrollAreaStyle	(Optional) Style used by the scroll content area.
		 */
		static GUIScrollArea* create(const GUIOptions& options, const String& scrollBarStyle = StringUtil::BLANK, 
			const String& scrollAreaStyle = StringUtil::BLANK);

		/**
		 * @copydoc	GUIElementContainer::getElementType
		 */
		virtual ElementType _getElementType() const override { return ElementType::ScrollArea; }

		/**
		 * @brief	Returns the scroll area layout that you may use to add elements inside the scroll area.
		 */
		GUILayout& getLayout() const { return *mContentLayout; }

		/**
		 * @brief	Scrolls the area up by specified amount of pixels, if possible.
		 */
		void scrollUpPx(UINT32 pixels);

		/**
		 * @brief	Scrolls the area down by specified amount of pixels, if possible.
		 */
		void scrollDownPx(UINT32 pixels);

		/**
		 * @brief	Scrolls the area left by specified amount of pixels, if possible.
		 */
		void scrollLeftPx(UINT32 pixels);

		/**
		 * @brief	Scrolls the area right by specified amount of pixels, if possible.
		 */
		void scrollRightPx(UINT32 pixels);

		/**
		 * @brief	Scrolls the area up by specified percentage (ranging [0, 1]), if possible.
		 */
		void scrollUpPct(float percent);

		/**
		 * @brief	Scrolls the area down by specified percentage (ranging [0, 1]), if possible.
		 */
		void scrollDownPct(float percent);

		/**
		 * @brief	Scrolls the area left by specified percentage (ranging [0, 1]), if possible.
		 */
		void scrollLeftPct(float percent);

		/**
		 * @brief	Scrolls the area right by specified percentage (ranging [0, 1]), if possible.
		 */
		void scrollRightPct(float percent);

		/**
		 * @brief	Scrolls the contents to the specified position.
		 *			(0 meaning top-most part of the content is visible,
		 *			and 1 meaning bottom-most part is visible)
		 */
		void scrollToVertical(float pct);

		/**
		 * @brief	Scrolls the contents to the specified position.
		 *			(0 meaning left-most part of the content is visible,
		 *			and 1 meaning right-most part is visible)
		 */
		void scrollToHorizontal(float pct);

		/**
		 * @brief	Returns how much is the scroll area scrolled in the vertical direction.
		 *			Returned value represents percentage where 0 means no scrolling
		 *			is happening, and 1 means area is fully scrolled to the bottom.
		 */
		float getVerticalScroll() const;

		/**
		 * @brief	Returns how much is the scroll area scrolled in the horizontal direction.
		 *			Returned value represents percentage where 0 means no scrolling
		 *			is happening, and 1 means area is fully scrolled to the right.
		 */
		float getHorizontalScroll() const;

		/**
		 * @brief	Returns the bounds of the scroll area not including the scroll bars.
		 *			(i.e. only the portion that contains the contents).
		 */
		Rect2I getContentBounds();

		/**
		 * @brief	Number of pixels the scroll bar will occupy when active. This is width
		 *			for vertical scrollbar, and height for horizontal scrollbar.
		 */
		static const UINT32 ScrollBarWidth;
	protected:
		~GUIScrollArea();

		/**
		 * @copydoc GUIElementContainer::updateBounds
		 */
		virtual void updateClippedBounds() override;

		/**
		 * @copydoc	GUIElementBase::_calculateLayoutSizeRange
		 */
		LayoutSizeRange _calculateLayoutSizeRange() const override;

		/**
		 * @copydoc GUIElementContainer::_getOptimalSize
		 */
		Vector2I _getOptimalSize() const override;
	private:
		GUIScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const String& scrollBarStyle, const String& scrollAreaStyle, const GUIDimensions& dimensions);

		/**
		 * @copydoc	GUIElementContainer::mouseEvent
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev) override;

		/**
		 * @brief	Called when the vertical scrollbar moves. 
		 *
		 * @param	pct	Scrollbar position ranging [0, 1].
		 */
		void vertScrollUpdate(float pct);

		/**
		 * @brief	Called when the horizontal scrollbar moves. 
		 *
		 * @param	pct	Scrollbar position ranging [0, 1].
		 */
		void horzScrollUpdate(float pct);

		/**
		 * @copydoc	GUIElementContainer::_updateLayoutInternal
		 */
		void _updateLayoutInternal(const GUILayoutData& data) override;

		/**
		 * @copydoc	GUIElementContainer::_getElementAreas
		 */
		void _getElementAreas(const Rect2I& layoutArea, Rect2I* elementAreas, UINT32 numElements, 
			const Vector<LayoutSizeRange>& sizeRanges, const LayoutSizeRange& mySizeRange) const override;

		/**
		 * @copydoc	GUIElementContainer::_getElementAreas
		 *
		 * @note	Also calculates some scroll area specific values.
		 */
		void _getElementAreas(const Rect2I& layoutArea, Rect2I* elementAreas, UINT32 numElements, 
			const Vector<LayoutSizeRange>& sizeRanges, Vector2I& visibleSize, Vector2I& contentSize) const;

		ScrollBarType mVertBarType;
		ScrollBarType mHorzBarType;
		String mScrollBarStyle;

		GUILayout* mContentLayout;
		GUIScrollBarVert* mVertScroll;
		GUIScrollBarHorz* mHorzScroll;

		float mVertOffset;
		float mHorzOffset;

		Vector2I mVisibleSize;
		Vector2I mContentSize;

		static const UINT32 MinHandleSize;
		static const UINT32 WheelScrollAmount;
	};
}