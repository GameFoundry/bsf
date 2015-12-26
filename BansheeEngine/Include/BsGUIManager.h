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
	class GUIManagerCore;

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
	 *
	 *			Internal class. Unless modifying internal engine systems you should have no need to access this class.
	 */
	class BS_EXPORT GUIManager : public Module<GUIManager>
	{
		/**
		 * @brief	Valid states of a drag and drop operation
		 */
		enum class DragState
		{
			NoDrag,
			HeldWithoutDrag,
			Dragging
		};

		/**
		 * @brief	GUI render data for a single viewport.
		 */
		struct GUIRenderData
		{
			GUIRenderData()
				:isDirty(true)
			{ }

			Vector<TransientMeshPtr> cachedMeshes;
			Vector<GUIMaterialInfo> cachedMaterials;
			Vector<CGUIWidget*> cachedWidgetsPerMesh;
			Vector<CGUIWidget*> widgets;
			bool isDirty;
		};

		/**
		 * @brief	Render data for a single GUI group used for notifying the core GUI renderer.
		 */
		struct GUICoreRenderData
		{
			SPtr<TransientMeshCore> mesh;
			SPtr<MaterialCore> material;
			Matrix4 worldTransform;
		};

		/**
		 * @brief	Container for a GUI widget.
		 */
		struct WidgetInfo
		{
			WidgetInfo(CGUIWidget* _widget)
				:widget(_widget)
			{ }

			CGUIWidget* widget;
		};

		/**
		 * @brief	Container for data about a single GUI element and its widget.
		 */
		struct ElementInfo
		{
			ElementInfo(GUIElement* element, CGUIWidget* widget)
				:element(element), widget(widget)
			{ }

			GUIElement* element;
			CGUIWidget* widget;
		};

		/**
		 * @brief	Container for data about a single GUI element and its widget currently under the pointer.
		 */
		struct ElementInfoUnderPointer
		{
			ElementInfoUnderPointer(GUIElement* element, CGUIWidget* widget)
				:element(element), widget(widget), usesMouseOver(false), 
				receivedMouseOver(false), isHovering(false)
			{ }

			GUIElement* element;
			CGUIWidget* widget;
			bool usesMouseOver;
			bool receivedMouseOver;
			bool isHovering;
		};

		/**
		 * @brief	Container for GUI element in focus.
		 */
		struct ElementFocusInfo
		{
			GUIElement* element;
			bool focus;
		};

	public:
		GUIManager();
		~GUIManager();

		/**
		 * @brief	Registers a newly created widget with the GUI manager.
		 *			This should be called by every GUI widget on creation.
		 */
		void registerWidget(CGUIWidget* widget);

		/**
		 * @brief	Unregisters a GUI widget from the GUI manager.
		 *			This should be called by every GUI widget before getting deleted.
		 */
		void unregisterWidget(CGUIWidget* widget);

		/**
		 * @brief	Called once per frame.
		 */
		void update();

		/**
		 * @brief	Queues the GUI element for destruction. Element will be destroyed during the next
		 *			call to update().
		 */
		void queueForDestroy(GUIElement* element);

		/**
		 * @brief	Change the GUI element focus state.
		 */
		void setFocus(GUIElement* element, bool focus);

		/**
		 * @brief	Changes the color of the input caret used in input boxes and similar controls.
		 */
		void setCaretColor(const Color& color) { mCaretColor = color; updateCaretTexture(); }

		/**
		 * @brief	Changes the text selection highlight color used in input boxes and similar controls.
		 */
		void setTextSelectionColor(const Color& color) { mTextSelectionColor = color; updateTextSelectionTexture(); }

		/**
		 * @brief	Returns the default caret texture used for rendering the input caret sprite.
		 */
		const HSpriteTexture& getCaretTexture() const { return mCaretTexture; }

		/**
		 * @brief	Returns the default selection highlight texture used for rendering the selection highlight sprites.
		 */
		const HSpriteTexture& getTextSelectionTexture() const { return mTextSelectionTexture; }

		/**
		 * @brief	Checks is the input caret visible this frame.
		 */
		bool getCaretBlinkState() const { return mIsCaretOn; }

		/**
		 * @brief	Returns input caret helper tool that allows you to easily position and show
		 *			an input caret in your GUI controls.
		 */
		GUIInputCaret* getInputCaretTool() const { return mInputCaret; }

		/**
		 * @brief	Returns input selection helper tool that allows you to easily position and show
		 *			an input selection highlight in your GUI controls.
		 */
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

		/**
		 * @brief	Gets the core thread portion of the GUI manager, responsible for rendering
		 * 			of GUI elements.
		 */
		GUIManagerCore* getCore() const { return mCore.load(std::memory_order_relaxed); }

	private:
		friend class GUIManagerCore;

		/**
		 * @brief	Recreates all dirty GUI meshes and makes them ready for rendering.
		 */
		void updateMeshes();

		/**
		 * @brief	Recreates the input caret texture.
		 */
		void updateCaretTexture();

		/**
		 * @brief	Recreates the input text selection highlight texture.
		 */
		void updateTextSelectionTexture();

		/**
		 * @brief	Destroys the core thread counterpart of the GUI manager.
		 *
		 * @param	core	Previously constructed core thread GUI manager instance.
		 */
		void destroyCore(GUIManagerCore* core);

		/**
		 * @brief	Destroys any elements or widgets queued for destruction.
		 *
		 * @note	Returns true if more elements have been added for destruction (will happen when destruction
		 *			of one element queues up destruction of another).
		 */
		bool processDestroyQueue();

		/**
		 * @brief	Finds a GUI element under the pointer at the specified screen position. This method will also
		 *			trigger pointer move/hover/leave events.
		 *
		 * @param	screenMousePos	Position of the pointer in screen coordinates.
		 * @param	buttonStates	States of the three mouse buttons (left, right, middle).
		 * @param	shift			Is shift key held.
		 * @param	control			Is control key held.
		 * @param	alt				Is alt key held.
		 */
		bool findElementUnderPointer(const Vector2I& screenMousePos, bool buttonStates[3], bool shift, bool control, bool alt);

		/**
		 * @brief	Called whenever a pointer (e.g. mouse cursor) is moved.
		 */
		void onPointerMoved(const PointerEvent& event);

		/**
		 * @brief	Called whenever a pointer button (e.g. mouse button) is released.
		 */
		void onPointerReleased(const PointerEvent& event);

		/**
		 * @brief	Called whenever a pointer button (e.g. mouse button) is pressed.
		 */
		void onPointerPressed(const PointerEvent& event);

		/**
		 * @brief	Called whenever a pointer button (e.g. mouse button) is double clicked.
		 */
		void onPointerDoubleClick(const PointerEvent& event);

		/**
		 * @brief	Called whenever a text is input.
		 */
		void onTextInput(const TextInputEvent& event);

		/**
		 * @brief	Called whenever an input command is input.
		 */
		void onInputCommandEntered(InputCommandType commandType);

		/**
		 * @brief	Called whenever a virtual button is pressed.
		 */
		void onVirtualButtonDown(const VirtualButton& button, UINT32 deviceIdx);

		/**
		 * @brief	Called by the drag and drop managed to notify us the drag ended.
		 */
		void onMouseDragEnded(const PointerEvent& event, DragCallbackInfo& dragInfo);

		/**
		 * @brief	Called when the specified window gains focus.
		 */
		void onWindowFocusGained(RenderWindow& win);

		/**
		 * @brief	Called when the specified window loses focus.
		 */
		void onWindowFocusLost(RenderWindow& win);

		/**
		 * @brief	Called when the mouse leaves the specified window.
		 */
		void onMouseLeftWindow(RenderWindow& win);

		/**
		 * @brief	Converts pointer buttons to mouse buttons.
		 */
		GUIMouseButton buttonToGUIButton(PointerEventButton pointerButton) const;

		/**
		 * @brief	Converts screen coordinates to coordinates relative to the specified widget.
		 */
		Vector2I getWidgetRelativePos(const CGUIWidget* widget, const Vector2I& screenPos) const;

		/**
		 * @brief	Converts window coordinates to coordinates relative to the specified bridged widget.
		 *			Returned coordinates will be relative to the bridge element.
		 *
		 * @param	If provided widget has no bridge, coordinates are returned as is.
		 */
		Vector2I windowToBridgedCoords(const CGUIWidget& widget, const Vector2I& windowPos) const;

		/**
		 * @brief	Returns the parent render window of the specified widget.
		 */
		const RenderWindow* getWidgetWindow(const CGUIWidget& widget) const;

		/**
		 * @brief	Hides the tooltip if any is shown.
		 */
		void hideTooltip();

		/**
		 * @brief	Sends a mouse event to the specified GUI element.
		 *
		 * @param	element	Element to send the event to.
		 * @param	event	Event data.
		 */
		bool sendMouseEvent(GUIElement* element, const GUIMouseEvent& event);

		/**
		 * @brief	Sends a text input event to the specified GUI element.
		 *
		 * @param	element	Element to send the event to.
		 * @param	event	Event data.
		 */
		bool sendTextInputEvent(GUIElement* element, const GUITextInputEvent& event);

		/**
		 * @brief	Sends a command event to the specified GUI element.
		 *
		 * @param	element	Element to send the event to.
		 * @param	event	Event data.
		 */
		bool sendCommandEvent(GUIElement* element, const GUICommandEvent& event);

		/**
		 * @brief	Sends a virtual button event to the specified GUI element.
		 *
		 * @param	element	Element to send the event to.
		 * @param	event	Event data.
		 */
		bool sendVirtualButtonEvent(GUIElement* element, const GUIVirtualButtonEvent& event);

		static const UINT32 DRAG_DISTANCE;
		static const float TOOLTIP_HOVER_TIME;

		static const UINT32 MESH_HEAP_INITIAL_NUM_VERTS;
		static const UINT32 MESH_HEAP_INITIAL_NUM_INDICES;

		Vector<WidgetInfo> mWidgets;
		UnorderedMap<const Viewport*, GUIRenderData> mCachedGUIData;
		MeshHeapPtr mMeshHeap;

		std::atomic<GUIManagerCore*> mCore;
		bool mCoreDirty;

		VertexDataDescPtr mVertexDesc;

		Stack<GUIElement*> mScheduledForDestruction;

		// Element and widget pointer is currently over
		Vector<ElementInfoUnderPointer> mElementsUnderPointer;
		Vector<ElementInfoUnderPointer> mNewElementsUnderPointer;

		// Element and widget that's being clicked on
		GUIMouseButton mActiveMouseButton;
		Vector<ElementInfo> mActiveElements;
		Vector<ElementInfo> mNewActiveElements;

		// Element and widget that currently have the keyboard focus
		Vector<ElementInfo> mElementsInFocus;
		Vector<ElementInfo> mNewElementsInFocus;

		Vector<ElementFocusInfo> mForcedFocusElements;

		// Tooltip
		bool mShowTooltip;
		float mTooltipElementHoverStart;

		GUIInputCaret* mInputCaret;
		GUIInputSelection* mInputSelection;

		bool mSeparateMeshesByWidget;
		Vector2I mLastPointerScreenPos;

		DragState mDragState;
		Vector2I mLastPointerClickPos;
		Vector2I mDragStartPos;

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
	};

	/**
	 * @brief	Handles GUI rendering on the core thread.
	 */
	class BS_EXPORT GUIManagerCore
	{
		friend class GUIManager;

		/**
		 * @brief	Contains mesh & material data for a group of GUI elements.
		 */
		struct RenderData
		{
			SPtr<TransientMeshCore> mesh;
			SPtr<MaterialCore> material;
			Matrix4 worldTransform;

			MaterialParamMat4Core worldTransformParam;
			MaterialParamFloatCore invViewportWidthParam;
			MaterialParamFloatCore invViewportHeightParam;
		};
	public:
		~GUIManagerCore();

	private:
		/**
		 * @brief	Updates the internal data that determines what will be rendered on the next
		 *			::render call.
		 *
		 * @param	data	GUI mesh/material per viewport.
		 */
		void updateData(const UnorderedMap<SPtr<CameraCore>, Vector<GUIManager::GUICoreRenderData>>& perCameraData);

		/**
		 * @brief	Triggered by the Renderer when the GUI should be rendered.
		 */
		void render(const SPtr<CameraCore>& camera);

		UnorderedMap<SPtr<CameraCore>, Vector<RenderData>> mPerCameraData;
	};

	/**
	 * @copydoc	GUIManager
	 */
	BS_EXPORT GUIManager& gGUIManager();
}