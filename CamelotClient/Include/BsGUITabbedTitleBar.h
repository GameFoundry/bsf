#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElement.h"
#include "CmRectI.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class GUITabbedTitleBar : public BS::GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUITabbedTitleBar* create(BS::GUIWidget& parent, CM::RenderWindow* parentWindow,
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* tabBtnStyle = nullptr, 
			BS::GUIElementStyle* minBtnStyle = nullptr, BS::GUIElementStyle* closeBtnStyle = nullptr);

		static GUITabbedTitleBar* create(BS::GUIWidget& parent, CM::RenderWindow* parentWindow, const BS::GUILayoutOptions& layoutOptions);
		static GUITabbedTitleBar* create(BS::GUIWidget& parent, CM::RenderWindow* parentWindow, const BS::GUILayoutOptions& layoutOptions, 
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* tabBtnStyle = nullptr, 
			BS::GUIElementStyle* minBtnStyle = nullptr, BS::GUIElementStyle* closeBtnStyle = nullptr);

		void addTab(const CM::HString& name);
		void insertTab(CM::UINT32 idx, const CM::HString& name);
		void removeTab(CM::UINT32 idx);

		boost::signal<void(CM::UINT32)> onTabActivated;
		boost::signal<void(CM::UINT32)> onTabClosed;
		boost::signal<void(CM::UINT32)> onTabDraggedOff;
		boost::signal<void(CM::UINT32)> onTabDraggedOn;

	protected:
		virtual ~GUITabbedTitleBar();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual CM::UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const BS::GUIMaterialInfo& getMaterial(CM::UINT32 renderElementIdx) const;

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
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);
	protected:
		static const CM::UINT32 TAB_SPACING;
		static const CM::UINT32 OPTION_BTN_SPACING;

		CM::Vector<GUITabButton*>::type mTabButtons;

		CM::UINT32 mUniqueTabIdx;
		CM::UINT32 mActiveTabIdx;
		BS::GUIWidget* mParentWidget;
		BS::GUITexture* mBackgroundImage;
		BS::GUIButton* mMinBtn;
		BS::GUIButton* mCloseBtn;
		BS::GUIToggleGroupPtr mTabToggleGroup;
		CM::RenderWindow* mParentWindow;

		bool mDragInProgress;
		GUITabButton* mDraggedBtn;
		CM::INT32 mDragBtnOffset;
		CM::INT32 mInitialDragOffset;

		const BS::GUIElementStyle* mBackgroundStyle;
		const BS::GUIElementStyle* mCloseBtnStyle;
		const BS::GUIElementStyle* mMinimizeBtnStyle;
		const BS::GUIElementStyle* mTabBtnStyle;

		GUITabbedTitleBar(BS::GUIWidget& parent, CM::RenderWindow* parentWindow, BS::GUIElementStyle* backgroundStyle, BS::GUIElementStyle* tabBtnStyle, 
			BS::GUIElementStyle* minBtnStyle, BS::GUIElementStyle* closeBtnStyle, const BS::GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);
		virtual CM::Vector2I _getOptimalSize() const;
		void _changeParentWidget(BS::GUIWidget* widget);

		void tabToggled(CM::UINT32 tabIdx);
		void tabClosed();
		void tabDragged(CM::UINT32 tabIdx, const CM::Vector2I& dragPos);
		void tabDragEnd(CM::UINT32 tabIdx, const CM::Vector2I& dragPos);
		void tabDraggedOn(CM::UINT32 tabIdx);

		CM::INT32 uniqueIdxToSeqIdx(CM::UINT32 uniqueIdx) const;
	};
}