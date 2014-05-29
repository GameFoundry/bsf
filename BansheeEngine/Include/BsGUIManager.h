#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMouseEvent.h"
#include "BsGUITextInputEvent.h"
#include "BsGUICommandEvent.h"
#include "BsGUIVirtualButtonEvent.h"
#include "BsGUIMaterialInfo.h"
#include "BsModule.h"
#include "BsColor.h"
#include "BsInput.h"
#include "BsEvent.h"

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
	class BS_EXPORT GUIManager : public Module<GUIManager>
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

			Vector<TransientMeshPtr> cachedMeshes;
			Vector<GUIMaterialInfo> cachedMaterials;
			Vector<GUIWidget*> cachedWidgetsPerMesh;
			Vector<GUIWidget*> widgets;
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
		void render(ViewportPtr& target, RenderQueue& renderQueue) const;

		void queueForDestroy(GUIElement* element);

		void setFocus(GUIElement* element, bool focus);

		void setCaretColor(const Color& color) { mCaretColor = color; updateCaretTexture(); }
		void setTextSelectionColor(const Color& color) { mTextSelectionColor = color; updateTextSelectionTexture(); }
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
		void setInputBridge(const RenderTexture* renderTex, const GUIElement* element);

	private:
		static const UINT32 DRAG_DISTANCE;

		static const UINT32 MESH_HEAP_INITIAL_NUM_VERTS;
		static const UINT32 MESH_HEAP_INITIAL_NUM_INDICES;

		Vector<WidgetInfo> mWidgets;
		UnorderedMap<const Viewport*, GUIRenderData> mCachedGUIData;
		MeshHeapPtr mMeshHeap;

		VertexDataDescPtr mVertexDesc;

		Stack<GUIElement*> mScheduledForDestruction;

		// Element and widget pointer is currently over
		Vector<ElementInfo> mElementsUnderPointer;
		Vector<ElementInfo> mNewElementsUnderPointer;

		// Element and widget that's being clicked on
		GUIMouseButton mActiveMouseButton;
		Vector<ElementInfo> mActiveElements;
		Vector<ElementInfo> mNewActiveElements;

		// Element and widget that currently have the keyboard focus
		Vector<ElementInfo> mElementsInFocus;
		Vector<ElementInfo> mNewElementsInFocus;

		Vector<ElementFocusInfo> mForcedFocusElements;

		GUIInputCaret* mInputCaret;
		GUIInputSelection* mInputSelection;

		bool mSeparateMeshesByWidget;
		Vector2I mLastPointerScreenPos;

		DragState mDragState;
		Vector2I mLastPointerClickPos;

		GUIMouseEvent mMouseEvent;
		GUITextInputEvent mTextInputEvent;
		GUICommandEvent mCommandEvent;
		GUIVirtualButtonEvent mVirtualButtonEvent;

		HSpriteTexture mCaretTexture;
		Color mCaretColor;
		float mCaretBlinkInterval;
		float mCaretLastBlinkTime;
		bool mIsCaretOn;
		CursorType mActiveCursor;

		HSpriteTexture mTextSelectionTexture;
		Color mTextSelectionColor;

		Map<const RenderTexture*, const GUIElement*> mInputBridge;

		HEvent mOnPointerMovedConn;
		HEvent mOnPointerPressedConn;
		HEvent mOnPointerReleasedConn;
		HEvent mOnPointerDoubleClick;
		HEvent mOnTextInputConn;
		HEvent mOnInputCommandConn;
		HEvent mOnVirtualButtonDown;

		HEvent mDragEndedConn;

		HEvent mWindowGainedFocusConn;
		HEvent mWindowLostFocusConn;

		HEvent mMouseLeftWindowConn;

		void updateMeshes();
		void updateCaretTexture();
		void updateTextSelectionTexture();
		void processDestroyQueue();

		bool findElementUnderPointer(const Vector2I& screenMousePos, bool buttonStates[3], bool shift, bool control, bool alt);

		void onPointerMoved(const PointerEvent& event);
		void onPointerReleased(const PointerEvent& event);
		void onPointerPressed(const PointerEvent& event);
		void onPointerDoubleClick(const PointerEvent& event);
		void onTextInput(const TextInputEvent& event);
		void onInputCommandEntered(InputCommandType commandType);
		void onVirtualButtonDown(const VirtualButton& button);

		void onMouseDragEnded(const PointerEvent& event, DragCallbackInfo& dragInfo);

		void onWindowFocusGained(RenderWindow& win);
		void onWindowFocusLost(RenderWindow& win);

		void onMouseLeftWindow(RenderWindow* win);

		GUIMouseButton buttonToGUIButton(PointerEventButton pointerButton) const;
		Vector2I getWidgetRelativePos(const GUIWidget& widget, const Vector2I& screenPos) const;
		Vector2I windowToBridgedCoords(const GUIWidget& widget, const Vector2I& windowPos) const;
		const RenderWindow* getWidgetWindow(const GUIWidget& widget) const;

		bool sendMouseEvent(GUIWidget* widget, GUIElement* element, const GUIMouseEvent& event);
		bool sendTextInputEvent(GUIWidget* widget, GUIElement* element, const GUITextInputEvent& event);
		bool sendCommandEvent(GUIWidget* widget, GUIElement* element, const GUICommandEvent& event);
		bool sendVirtualButtonEvent(GUIWidget* widget, GUIElement* element, const GUIVirtualButtonEvent& event);
	};

	BS_EXPORT GUIManager& gGUIManager();
}