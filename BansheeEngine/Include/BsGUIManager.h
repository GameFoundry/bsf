#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMouseEvent.h"
#include "BsGUITextInputEvent.h"
#include "BsGUICommandEvent.h"
#include "BsGUIVirtualButtonEvent.h"
#include "BsGUIMaterialInfo.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmInput.h"
#include <boost/signals/connection.hpp>

namespace BansheeEngine
{
	/**
	 * @brief	Manages the rendering and input of all GUI widgets in the scene. 
	 * 			
	 * @note	If adding or modifying GUIManager functionality ensure that GUIManager data never gets modified
	 * 			outside of update() method or Input callbacks. If you need such functionality add temporary variables
	 * 			that store you changes and then execute them delayed in update().  
	 * 			
	 *			This ensures that GUIElements don't recursively modify GUIManager while GUIManager is still using that data.
	 *			
	 *			e.g. setFocus usually gets called from within GUIElements, however we don't want elements in focus be modified immediately 
	 *			since that setFocus call could have originated in sendCommandEvent and elements in focus array would be modified while
	 *			still being iterated upon.
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

			CM::Vector<CM::TransientMeshPtr>::type cachedMeshes;
			CM::Vector<GUIMaterialInfo>::type cachedMaterials;
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

		struct ElementInfo
		{
			ElementInfo(GUIElement* element, GUIWidget* widget)
				:element(element), widget(widget)
			{ }

			GUIElement* element;
			GUIWidget* widget;
		};

		struct ElementFocusInfo
		{
			GUIElement* element;
			bool focus;
		};

	public:
		GUIManager();
		~GUIManager();

		void registerWidget(GUIWidget* widget);
		void unregisterWidget(GUIWidget* widget);

		void update();
		void render(CM::ViewportPtr& target, CM::RenderQueue& renderQueue) const;

		void queueForDestroy(GUIElement* element);

		void setFocus(GUIElement* element, bool focus);

		void setCaretColor(const CM::Color& color) { mCaretColor = color; updateCaretTexture(); }
		void setTextSelectionColor(const CM::Color& color) { mTextSelectionColor = color; updateTextSelectionTexture(); }
		const HSpriteTexture& getCaretTexture() const { return mCaretTexture; }
		const HSpriteTexture& getTextSelectionTexture() const { return mTextSelectionTexture; }
		bool getCaretBlinkState() const { return mIsCaretOn; }

		GUIInputCaret* getInputCaretTool() const { return mInputCaret; }
		GUIInputSelection* getInputSelectionTool() const { return mInputSelection; }

		/**
		 * @brief	Allows you to bridge GUI input from a GUI element into another render target.
		 *
		 * @param	renderTex 	The render target to which to bridge the input.
		 * @param	element		The element from which to bridge input. Input will be transformed according to this
		 * 						elements position and size. Provide nullptr if you want to remove a bridge for the specified widget.
		 * 					
		 * @note	This is useful if you use render textures, where your GUI is rendered off-
		 * 			screen. In such case you need to display the render texture within another GUIElement
		 * 			in a GUIWidget, but have no way of sending input to the render texture (normally
		 * 			input is only sent to render windows). This allows you to change that - any GUIWidget
		 * 			using the bridged render texture as a render target will then receive input when mouse
		 * 			is over the specified element.
		 * 			
		 *			Bridged element needs to remove itself as the bridge when it is destroyed.
		 */
		void setInputBridge(const CM::RenderTexture* renderTex, const GUIElement* element);

	private:
		static const CM::UINT32 DRAG_DISTANCE;

		static const CM::UINT32 MESH_HEAP_INITIAL_NUM_VERTS;
		static const CM::UINT32 MESH_HEAP_INITIAL_NUM_INDICES;

		CM::Vector<WidgetInfo>::type mWidgets;
		CM::UnorderedMap<const CM::Viewport*, GUIRenderData>::type mCachedGUIData;
		CM::MeshHeapPtr mMeshHeap;

		CM::VertexDataDescPtr mVertexDesc;

		CM::Stack<GUIElement*>::type mScheduledForDestruction;

		// Element and widget mouse is currently over
		CM::Vector<ElementInfo>::type mElementsUnderCursor;
		CM::Vector<ElementInfo>::type mNewElementsUnderCursor;

		// Element and widget that's being clicked on
		GUIMouseButton mActiveMouseButton;
		CM::Vector<ElementInfo>::type mActiveElements;
		CM::Vector<ElementInfo>::type mNewActiveElements;

		// Element and widget that currently have the keyboard focus
		CM::Vector<ElementInfo>::type mElementsInFocus;
		CM::Vector<ElementInfo>::type mNewElementsInFocus;

		CM::Vector<ElementFocusInfo>::type mForcedFocusElements;

		GUIInputCaret* mInputCaret;
		GUIInputSelection* mInputSelection;

		bool mSeparateMeshesByWidget;
		CM::Vector2I mLastCursorScreenPos;

		DragState mDragState;
		CM::Vector2I mLastCursorClickPos;

		GUIMouseEvent mMouseEvent;
		GUITextInputEvent mTextInputEvent;
		GUICommandEvent mCommandEvent;
		GUIVirtualButtonEvent mVirtualButtonEvent;

		HSpriteTexture mCaretTexture;
		CM::Color mCaretColor;
		float mCaretBlinkInterval;
		float mCaretLastBlinkTime;
		bool mIsCaretOn;

		HSpriteTexture mTextSelectionTexture;
		CM::Color mTextSelectionColor;

		CM::Map<const CM::RenderTexture*, const GUIElement*>::type mInputBridge;

		boost::signals::connection mOnCursorMovedConn;
		boost::signals::connection mOnCursorPressedConn;
		boost::signals::connection mOnCursorReleasedConn;
		boost::signals::connection mOnCursorDoubleClick;
		boost::signals::connection mOnTextInputConn;
		boost::signals::connection mOnInputCommandConn;
		boost::signals::connection mOnVirtualButtonDown;

		boost::signals::connection mDragEndedConn;

		boost::signals::connection mWindowGainedFocusConn;
		boost::signals::connection mWindowLostFocusConn;

		boost::signals::connection mMouseLeftWindowConn;

		void updateMeshes();
		void updateCaretTexture();
		void updateTextSelectionTexture();
		void processDestroyQueue();

		bool findElementUnderCursor(const CM::Vector2I& screenMousePos, bool buttonStates[3], bool shift, bool control, bool alt);

		void onCursorMoved(const CM::PositionalInputEvent& event);
		void onCursorReleased(const CM::PositionalInputEvent& event);
		void onCursorPressed(const CM::PositionalInputEvent& event);
		void onCursorDoubleClick(const CM::PositionalInputEvent& event);
		void onTextInput(const CM::TextInputEvent& event);
		void onInputCommandEntered(CM::InputCommandType commandType);
		void onVirtualButtonDown(const VirtualButton& button);

		bool onMouseDragEnded(const CM::PositionalInputEvent& event);

		void onWindowFocusGained(CM::RenderWindow& win);
		void onWindowFocusLost(CM::RenderWindow& win);

		void onMouseLeftWindow(CM::RenderWindow* win);

		GUIMouseButton buttonToGUIButton(CM::PositionalInputEventButton cursorButton) const;
		CM::Vector2I getWidgetRelativePos(const GUIWidget& widget, const CM::Vector2I& screenPos) const;
		CM::Vector2I windowToBridgedCoords(const GUIWidget& widget, const CM::Vector2I& windowPos) const;
		const CM::RenderWindow* getWidgetWindow(const GUIWidget& widget) const;

		bool sendMouseEvent(GUIWidget* widget, GUIElement* element, const GUIMouseEvent& event);
		bool sendTextInputEvent(GUIWidget* widget, GUIElement* element, const GUITextInputEvent& event);
		bool sendCommandEvent(GUIWidget* widget, GUIElement* element, const GUICommandEvent& event);
		bool sendVirtualButtonEvent(GUIWidget* widget, GUIElement* element, const GUIVirtualButtonEvent& event);
	};

	BS_EXPORT GUIManager& gGUIManager();
}