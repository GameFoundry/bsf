#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMouseEvent.h"
#include "BsGUITextInputEvent.h"
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
		enum class DragState
		{
			NoDrag,
			HeldWithoutDrag,
			Dragging
		};

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

		void queueForDestroy(GUIElement* element);

		void setCaretColor(const CM::Color& color) { mCaretColor = color; updateCaretTexture(); }
		void setTextSelectionColor(const CM::Color& color) { mTextSelectionColor = color; updateTextSelectionTexture(); }
		const SpriteTexturePtr& getCaretTexture() const { return mCaretTexture; }
		const SpriteTexturePtr& getTextSelectionTexture() const { return mTextSelectionTexture; }
		bool getCaretBlinkState() const { return mIsCaretOn; }

		GUIInputCaret* getInputCaretTool() const { return mInputCaret; }
		GUIInputSelection* getInputSelectionTool() const { return mInputSelection; }

		/**
		 * @brief	Selective input allows you to limit input only to certain GUI elements. After
		 * 			enabling selective input use addSelectiveInput* methods to add specific elements that
		 * 			will be allowed to receive input.
		 *
		 * @param	onOutsideClickCallback	Callback that gets triggered when the user
		 * 									presses a mouse button outside of the selective input area.
		 */
		void enableSelectiveInput(std::function<void()> onOutsideClickCallback);

		/**
		 * @brief	Disables selective input and clears any selective input elements.
		 */
		void disableSelectiveInput();

		void addSelectiveInputWidget(const GUIWidget* widget);
		void addSelectiveInputElement(const GUIElement* element);

		boost::signal<void(GUIWidget*, GUIElement*, const GUIMouseEvent&)> mouseEventFilter;
		boost::signal<void(GUIWidget*, GUIElement*, const GUITextInputEvent&)> textInputEventFilter;
	private:
		struct SelectiveInputData
		{
			SelectiveInputData()
				:acceptAllElements(false)
			{ }

			CM::Set<const GUIElement*>::type elements;
			bool acceptAllElements;
		};

		static CM::UINT32 DRAG_DISTANCE;

		CM::Vector<WidgetInfo>::type mWidgets;
		CM::UnorderedMap<const CM::Viewport*, GUIRenderData>::type mCachedGUIData;

		CM::Stack<GUIElement*>::type mScheduledForDestruction;

		// Element and widget mouse is currently over
		GUIWidget* mWidgetUnderCursor;
		GUIElement* mElementUnderCursor;

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

		DragState mDragState;
		CM::Int2 mLastCursorClickPos;

		GUIMouseEvent mMouseEvent;
		GUITextInputEvent mTextInputEvent;
		GUICommandEvent mCommandEvent;

		SpriteTexturePtr mCaretTexture;
		CM::Color mCaretColor;
		float mCaretBlinkInterval;
		float mCaretLastBlinkTime;
		bool mIsCaretOn;

		SpriteTexturePtr mTextSelectionTexture;
		CM::Color mTextSelectionColor;

		// Selective input
		bool mSelectiveInputActive;
		CM::Map<const GUIWidget*, SelectiveInputData>::type mSelectiveInputData;
		std::function<void()> mOnOutsideClickCallback;

		boost::signals::connection mOnCursorMovedConn;
		boost::signals::connection mOnCursorPressedConn;
		boost::signals::connection mOnCursorReleasedConn;
		boost::signals::connection mOnCursorDoubleClick;
		boost::signals::connection mOnTextInputConn;
		boost::signals::connection mOnInputCommandConn;

		boost::signals::connection mDragEndedConn;

		boost::signals::connection mWindowGainedFocusConn;
		boost::signals::connection mWindowLostFocusConn;

		boost::signals::connection mMouseLeftWindowConn;

		void updateMeshes();
		void updateCaretTexture();
		void updateTextSelectionTexture();
		void processDestroyQueue();

		bool findElementUnderCursor(const CM::Int2& screenMousePos, bool buttonStates[3], bool shift, bool control, bool alt);
		bool handleCursorOver(GUIWidget* widget, GUIElement* element, const CM::Int2& screenMousePos, 
			bool buttonStates[3], bool shift, bool control, bool alt);

		void onCursorMoved(const CM::PositionalInputEvent& event);
		void onCursorReleased(const CM::PositionalInputEvent& event);
		void onCursorPressed(const CM::PositionalInputEvent& event);
		void onCursorDoubleClick(const CM::PositionalInputEvent& event);
		void onTextInput(const CM::TextInputEvent& event);
		void onInputCommandEntered(CM::InputCommandType commandType);

		bool onMouseDragEnded(const CM::PositionalInputEvent& event);

		void onWindowFocusGained(CM::RenderWindow& win);
		void onWindowFocusLost(CM::RenderWindow& win);

		void onMouseLeftWindow(CM::RenderWindow* win);

		GUIMouseButton buttonToGUIButton(CM::PositionalInputEventButton cursorButton) const;
		CM::Int2 getWidgetRelativePos(const GUIWidget& widget, const CM::Int2& screenPos) const;

		bool sendMouseEvent(GUIWidget* widget, GUIElement* element, const GUIMouseEvent& event);
		bool sendTextInputEvent(GUIWidget* widget, GUIElement* element, const GUITextInputEvent& event);
		bool sendCommandEvent(GUIWidget* widget, GUIElement* element, const GUICommandEvent& event);
	};

	BS_EXPORT GUIManager& gGUIManager();
}