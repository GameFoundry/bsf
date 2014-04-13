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
		static const CM::String& getGUITypeName();

		static GUIScrollArea* create(ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const CM::String& scrollBarStyle = CM::StringUtil::BLANK, const CM::String& scrollAreaStyle = CM::StringUtil::BLANK);

		static GUIScrollArea* create(ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const GUIOptions& layoutOptions, const CM::String& scrollBarStyle = CM::StringUtil::BLANK, 
			const CM::String& scrollAreaStyle = CM::StringUtil::BLANK);

		static GUIScrollArea* create(const CM::String& scrollBarStyle = CM::StringUtil::BLANK, 
			const CM::String& scrollAreaStyle = CM::StringUtil::BLANK);

		static GUIScrollArea* create(const GUIOptions& layoutOptions, const CM::String& scrollBarStyle = CM::StringUtil::BLANK, 
			const CM::String& scrollAreaStyle = CM::StringUtil::BLANK);

		virtual ElementType getElementType() const { return ElementType::ScrollArea; }

		void scrollUpPx(CM::UINT32 pixels);
		void scrollDownPx(CM::UINT32 pixels);

		void scrollLeftPx(CM::UINT32 pixels);
		void scrollRightPx(CM::UINT32 pixels);

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
			const CM::String& scrollBarStyle, const CM::String& scrollAreaStyle, const GUILayoutOptions& layoutOptions);

		ScrollBarType mVertBarType;
		ScrollBarType mHorzBarType;
		CM::String mScrollBarStyle;

		GUILayout* mContentLayout;
		GUIScrollBarVert* mVertScroll;
		GUIScrollBarHorz* mHorzScroll;

		float mVertOffset;
		float mHorzOffset;

		CM::UINT32 mClippedContentWidth, mClippedContentHeight;
		CM::UINT32 mContentWidth, mContentHeight;

		static const CM::UINT32 ScrollBarWidth;
		static const CM::UINT32 MinHandleSize;
		static const CM::UINT32 WheelScrollAmount;

		virtual bool mouseEvent(const GUIMouseEvent& ev);

		void scrollToVertical(float pct);
		void scrollToHorizontal(float pct);

		void vertScrollUpdate(float pct);
		void horzScrollUpdate(float pct);
		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

	};
}