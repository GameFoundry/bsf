#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	enum class ScrollBarType
	{
		ShowIfDoesntFit,
		AlwaysShow,
		NeverShow
	};

	class BS_EXPORT GUIScrollArea : public GUIElementContainer
	{
	public:
		static const String& getGUITypeName();

		static GUIScrollArea* create(ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const String& scrollBarStyle = StringUtil::BLANK, const String& scrollAreaStyle = StringUtil::BLANK);

		static GUIScrollArea* create(ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const GUIOptions& layoutOptions, const String& scrollBarStyle = StringUtil::BLANK, 
			const String& scrollAreaStyle = StringUtil::BLANK);

		static GUIScrollArea* create(const String& scrollBarStyle = StringUtil::BLANK, 
			const String& scrollAreaStyle = StringUtil::BLANK);

		static GUIScrollArea* create(const GUIOptions& layoutOptions, const String& scrollBarStyle = StringUtil::BLANK, 
			const String& scrollAreaStyle = StringUtil::BLANK);

		virtual ElementType getElementType() const { return ElementType::ScrollArea; }

		void scrollUpPx(UINT32 pixels);
		void scrollDownPx(UINT32 pixels);

		void scrollLeftPx(UINT32 pixels);
		void scrollRightPx(UINT32 pixels);

		void scrollUpPct(float percent);
		void scrollDownPct(float percent);

		void scrollLeftPct(float percent);
		void scrollRightPct(float percent);

		GUILayout& getLayout() const { return *mContentLayout; }
	protected:
		~GUIScrollArea();

		/**
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();
	private:
		GUIScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const String& scrollBarStyle, const String& scrollAreaStyle, const GUILayoutOptions& layoutOptions);

		ScrollBarType mVertBarType;
		ScrollBarType mHorzBarType;
		String mScrollBarStyle;

		GUILayout* mContentLayout;
		GUIScrollBarVert* mVertScroll;
		GUIScrollBarHorz* mHorzScroll;

		float mVertOffset;
		float mHorzOffset;

		UINT32 mClippedContentWidth, mClippedContentHeight;
		UINT32 mContentWidth, mContentHeight;

		static const UINT32 ScrollBarWidth;
		static const UINT32 MinHandleSize;
		static const UINT32 WheelScrollAmount;

		virtual bool mouseEvent(const GUIMouseEvent& ev);

		void scrollToVertical(float pct);
		void scrollToHorizontal(float pct);

		void vertScrollUpdate(float pct);
		void horzScrollUpdate(float pct);
		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

	};
}