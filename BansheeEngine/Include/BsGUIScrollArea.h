#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"

namespace BansheeEngine
{
	enum class ScrollBarType
	{
		ShowIfDoesntFit,
		AlwaysShow,
		NeverShow
	};

	class BS_EXPORT GUIScrollArea : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIScrollArea* create(GUIWidget& parent, ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const GUIElementStyle* scrollBarStyle = nullptr, const GUIElementStyle* scrollAreaStyle = nullptr);

		static GUIScrollArea* create(GUIWidget& parent, ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const GUIOptions& layoutOptions, const GUIElementStyle* scrollBarStyle = nullptr, 
			const GUIElementStyle* scrollAreaStyle = nullptr);

		static GUIScrollArea* create(GUIWidget& parent, const GUIElementStyle* scrollBarStyle = nullptr, const GUIElementStyle* scrollAreaStyle = nullptr);

		static GUIScrollArea* create(GUIWidget& parent, const GUIOptions& layoutOptions, const GUIElementStyle* scrollBarStyle = nullptr, 
			const GUIElementStyle* scrollAreaStyle = nullptr);

		GUILayout& getLayout() const { return *mContentLayout; }

	protected:
		~GUIScrollArea();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual CM::UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const GUIMaterialInfo& getMaterial(CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads()
		 */
		virtual CM::UINT32 getNumQuads(CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer()
		 */
		virtual void fillBuffer(CM::UINT8* vertices, CM::UINT8* uv, CM::UINT32* indices, CM::UINT32 startingQuad, 
			CM::UINT32 maxNumQuads, CM::UINT32 vertexStride, CM::UINT32 indexStride, CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();

		/**
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();

		virtual CM::Vector2I _getOptimalSize() const;
	private:
		GUIScrollArea(GUIWidget& parent, ScrollBarType vertBarType, ScrollBarType horzBarType, 
			const GUIElementStyle* scrollBarStyle, const GUIElementStyle* scrollAreaStyle, const GUILayoutOptions& layoutOptions);

		ScrollBarType mVertBarType;
		ScrollBarType mHorzBarType;
		const GUIElementStyle* mScrollBarStyle;

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

		void vertScrollUpdate(float pct);
		void horzScrollUpdate(float pct);
		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		virtual void _changeParentWidget(GUIWidget* widget);
	};
}