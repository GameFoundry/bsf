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
			WidgetInfo(GUIWidget* _widget, const boost::signals::connection& _onAddedConn, const boost::signals::connection& _onRemovedConn)
				:widget(_widget), onAddedConn(_onAddedConn), onRemovedConn(_onRemovedConn)
			{ }

			GUIWidget* widget;
			boost::signals::connection onAddedConn;
			boost::signals::connection onRemovedConn;
		};

	public:
		GUIManager();
		~GUIManager();

		void registerWidget(GUIWidget* widget);
		void unregisterWidget(GUIWidget* widget);

		void update();
		void render(CM::ViewportPtr& target, CM::RenderQueue& renderQueue) const;

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

		boost::signals::connection mOnButtonDownConn;
		boost::signals::connection mOnButtonUpConn;
		boost::signals::connection mOnMouseMovedConn;
		boost::signals::connection mOnTextInputConn;

		boost::signals::connection mMouseDragEndedConn;

		boost::signals::connection mWindowGainedFocusConn;
		boost::signals::connection mWindowLostFocusConn;
		boost::signals::connection mWindowMovedOrResizedConn;

		void updateMeshes();
		void updateCaretTexture();
		void updateTextSelectionTexture();

		void onButtonDown(const CM::ButtonEvent& event);
		void onButtonUp(const CM::ButtonEvent& event);

		void onMouseMoved(const CM::MouseEvent& event);
		void onTextInput(const CM::TextInputEvent& event);

		bool onMouseDragEnded(const CM::ButtonEvent& event);

		void onWindowFocusGained(CM::RenderWindow& win);
		void onWindowFocusLost(CM::RenderWindow& win);
		void onWindowMovedOrResized(CM::RenderWindow& win);

		void onGUIElementAddedToWidget(GUIWidget* widget, GUIElement* element);
		void onGUIElementRemovedFromWidget(GUIWidget* widget, GUIElement* element);

		GUIMouseButton buttonToMouseButton(CM::ButtonCode code) const;
		CM::Int2 getWidgetRelativePos(const GUIWidget& widget, const CM::Int2& screenPos) const;

		bool sendMouseEvent(GUIWidget* widget, GUIElement* element, const GUIMouseEvent& event);
		bool sendKeyEvent(GUIWidget* widget, GUIElement* element, const GUIKeyEvent& event);
	};

	BS_EXPORT GUIManager& gGUIManager();
}