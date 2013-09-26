#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIButtonEvent.h"
#include "BsGUICommandEvent.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmInput.h"
#include <boost/signals/connection.hpp>

namespace BansheeEngine
{
	/**
	 * @brief	Manages the rendering and input of all GUI widgets in the scene. 
	 */
	class BS_EXPORT GUIManager : public CM::Module<GUIManager>
	{
		struct GUIRenderData
		{
			GUIRenderData()
				:isDirty(true)
			{ }

			CM::Vector<CM::HMesh>::type cachedMeshes;
			CM::Vector<CM::HMaterial>::type cachedMaterials;
			CM::Vector<GUIWidget*>::type cachedWidgetsPerMesh;
			CM::Vector<GUIWidget*>::type widgets;
			bool isDirty;
		};

		struct WidgetInfo
		{
			WidgetInfo(GUIWidget* _widget)
				:widget(_widget)
			{ }

			GUIWidget* widget;
		};

	public:
		GUIManager();
		~GUIManager();

		void registerWidget(GUIWidget* widget);
		void unregisterWidget(GUIWidget* widget);

		void update();
		void render(CM::ViewportPtr& target, CM::RenderQueue& renderQueue) const;

		void openDropDownListBox(const GUIListBox* parentList, const CM::Vector<CM::WString>::type& elements, 
			std::function<void(CM::UINT32)> selectedCallback, const GUISkin& skin);

		void openMenuBarMenu(GUIButton* parentButton, const GUIMenu* menu);

		void queueForDestroy(GUIElement* element);

		void setCaretColor(const CM::Color& color) { mCaretColor = color; updateCaretTexture(); }
		void setTextSelectionColor(const CM::Color& color) { mTextSelectionColor = color; updateTextSelectionTexture(); }
		const SpriteTexturePtr& getCaretTexture() const { return mCaretTexture; }
		const SpriteTexturePtr& getTextSelectionTexture() const { return mTextSelectionTexture; }
		bool getCaretBlinkState() const { return mIsCaretOn; }

		GUIInputCaret* getInputCaretTool() const { return mInputCaret; }
		GUIInputSelection* getInputSelectionTool() const { return mInputSelection; }

		boost::signal<void(GUIWidget*, GUIElement*, const GUIMouseEvent&)> mouseEventFilter;
		boost::signal<void(GUIWidget*, GUIElement*, const GUIKeyEvent&)> keyEventFilter;
	private:
		CM::Vector<WidgetInfo>::type mWidgets;
		CM::UnorderedMap<const CM::Viewport*, GUIRenderData>::type mCachedGUIData;

		CM::Stack<GUIElement*>::type mScheduledForDestruction;

		// Element and widget mouse is currently over
		GUIWidget* mMouseOverWidget;
		GUIElement* mMouseOverElement;

		// Element and widget that's being clicked on
		GUIWidget* mActiveWidget;
		GUIElement* mActiveElement;
		GUIMouseButton mActiveMouseButton;

		// Element and widget that currently have the keyboard focus
		GUIWidget* mKeyboardFocusWidget;
		GUIElement* mKeyboardFocusElement;

		GUIInputCaret* mInputCaret;
		GUIInputSelection* mInputSelection;

		bool mSeparateMeshesByWidget;
		CM::Int2 mLastCursorLocalPos;

		GUIMouseEvent mMouseEvent;
		GUIKeyEvent mKeyEvent;
		GUICommandEvent mCommandEvent;

		SpriteTexturePtr mCaretTexture;
		CM::Color mCaretColor;
		float mCaretBlinkInterval;
		float mCaretLastBlinkTime;
		bool mIsCaretOn;

		SpriteTexturePtr mTextSelectionTexture;
		CM::Color mTextSelectionColor;

		// Drop down box
		bool mDropDownBoxOpenScheduled;
		bool mDropDownBoxActive;
		bool mDropDownBoxCloseScheduled;
		CM::HSceneObject mDropDownSO;
		CM::GameObjectHandle<GUIDropDownBox> mDropDownBox;
		std::function<void(CM::UINT32)> mListBoxSelectionMade;

		boost::signals::connection mOnButtonDownConn;
		boost::signals::connection mOnButtonUpConn;
		boost::signals::connection mOnMouseMovedConn;
		boost::signals::connection mOnTextInputConn;

		boost::signals::connection mMouseDragEndedConn;

		boost::signals::connection mWindowGainedFocusConn;
		boost::signals::connection mWindowLostFocusConn;
		boost::signals::connection mWindowMovedOrResizedConn;

		boost::signals::connection mMouseLeftWindowConn;

		void updateMeshes();
		void updateCaretTexture();
		void updateTextSelectionTexture();
		void processDestroyQueue();

		void onButtonDown(const CM::ButtonEvent& event);
		void onButtonUp(const CM::ButtonEvent& event);

		void onMouseMoved(const CM::MouseEvent& event);
		void onTextInput(const CM::TextInputEvent& event);

		bool onMouseDragEnded(const CM::ButtonEvent& event);

		void onWindowFocusGained(CM::RenderWindow& win);
		void onWindowFocusLost(CM::RenderWindow& win);
		void onWindowMovedOrResized(CM::RenderWindow& win);

		void onMouseLeftWindow(CM::RenderWindow* win);

		bool handleMouseOver(GUIWidget* widget, GUIElement* element, const CM::Int2& screenMousePos, float wheelScrollAmount = 0.0f);

		void openContextMenu(const GUIContextMenu* menu, const CM::Int2& position, GUIWidget& widget);

		void closeDropDownListBox(CM::INT32 selectedIdx);
		void closeDropDownBox();

		GUIMouseButton buttonToMouseButton(CM::ButtonCode code) const;
		CM::Int2 getWidgetRelativePos(const GUIWidget& widget, const CM::Int2& screenPos) const;

		bool sendMouseEvent(GUIWidget* widget, GUIElement* element, const GUIMouseEvent& event);
		bool sendKeyEvent(GUIWidget* widget, GUIElement* element, const GUIKeyEvent& event);
	};

	BS_EXPORT GUIManager& gGUIManager();
}