#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	class DockOverlayRenderer;

	/**
	 * @brief	GUI element that allows editor widgets to be docked in it using arbitrary
	 *			layouts. Docked widgets can be resized, undocked, maximizes or closed as needed.
	 */
	class DockManager : public GUIElementContainer
	{
	public:
		/**
		 * @brief	Contains information about a single dock area. Each container can be a parent to
		 *			two other containers or may contain a widget, which results in a container hierarchy. 
		 *			Two children can be split vertically or horizontally at an user-defined point.
		 */
		class DockContainer
		{
		public:
			DockContainer(DockManager* manager);
			DockContainer(DockManager* manager, DockContainer* parent);
			~DockContainer();

			/**
			 * @brief	Determines the position and size of the container, relative to the parent dock manager.
			 */
			void setArea(INT32 x, INT32 y, UINT32 width, UINT32 height);

			/**
			 * @brief	Transforms the container from non-leaf (parent to other containers)
			 *			to leaf (parent to widgets). This involves creating a widget container to
			 *			which you can dock widgets to.
			 *
			 * @param	parentWindow	Editor window of the parent dock manager.
			 */
			void makeLeaf(EditorWindowBase* parentWindow);

			/**
			 * @brief	Transforms the container from non-leaf (parent to other containers)
			 *			to leaf (parent to widgets). Unlike the other overload this one accepts
			 *			a previously created widget container.
			 *
			 * @param	guiWidgetSO			Parent SceneObject of the GUIWidget used by the provided widget container.
			 * @param	existingContainer	An existing widget container that may be used for docking widgets.
			 */
			void makeLeaf(const HSceneObject& guiWidgetSO, EditorWidgetContainer* existingContainer);

			/**
			 * @brief	Splits a leaf container containing a widget container (or may be empty in the case of root with no elements) 
			 *			into a container parent to two other containers.
			 *
			 * @param	horizontal			Whether the split is horizontal (true) or vertical (false.
			 * @param	newChildIsFirst		Determines to which child should the widget container from this object be moved to.
			 *								If the new child is first, then bottom or right (for horizontal and vertical respectively)
			 *								will receive the current widget container, and opposite if it's not first.
			 * @param	splitPosition		Determines at what position(in percent) should this container be split. User can modify this later
			 *								via a dock slider.
			 */
			void splitContainer(bool horizontal, bool newChildIsFirst, float splitPosition = 0.5f);

			/**
			 * @brief	Splits a leaf container containing a widget container (or may be empty in the case of root with no elements)
			 *			into a container parent to two other containers. Unlike "splitContainer" new containers aren't created automatically
			 *			but you must provide existing ones. If this container is non-leaf its widget container will be destroyed.
			 *
			 * @param	first			Container to insert into the first child slot (left if vertical split, top if horizontal split).
			 * @param	second			Container to insert into the second child slot (right if vertical split, bottom if horizontal split).
			 * @param	horizontal		Whether the split is horizontal (true) or vertical (false.
			 * @param	splitPosition	Determines at what position(in percent) should this container be split. User can modify this later
			 *							via a dock slider.
			 */
			void makeSplit(DockContainer* first, DockContainer* second, bool horizontal, float splitPosition);

			/**
			 * @brief	Adds a new widget to the left side of the container. If the container is leaf it will
			 *			be split into two containers vertically.
			 */
			void addLeft(EditorWidgetBase* widget);

			/**
			 * @brief	Adds a new widget to the right side of the container. If the container is leaf it will
			 *			be split into two containers vertically.
			 */
			void addRight(EditorWidgetBase* widget);

			/**
			 * @brief	Adds a new widget to the top side of the container. If the container is leaf it will
			 *			be split into two containers horizontally.
			 */
			void addTop(EditorWidgetBase* widget);

			/**
			 * @brief	Adds a new widget to the bottom side of the container. If the container is leaf it will
			 *			be split into two containers horizontally.
			 */
			void addBottom(EditorWidgetBase* widget);

			/**
			 * @brief	Adds an existing widget to this leaf container.
			 */
			void addWidget(EditorWidgetBase* widget);

			/**
			 * @brief	Attempts to find a widget with the specified name, opens it
			 *			and adds it to this leaf container.
			 */
			void addWidget(const String& name);

			/**
			 * @brief	Update to be called once per frame. Calls updates on all child widgets.
			 */
			void update();

			/**
			 * @brief	Attempts to find an existing leaf dock container with the specified
			 *			widget container. Returns null if one cannot be found.
			 */
			DockContainer* find(EditorWidgetContainer* widgetContainer);

			/**
			 * @brief	Searches for a container at the specified position. Call this at this top-most
			 *			container in order to search them all.
			 *
			 * @param	pos	Position is relative to the container area. 
			 *
			 * @return	null if it fails, else the found DockContainer at position.
			 */
			DockContainer* findAtPos(const Vector2I& pos);

			/**
			 * @brief	Returns the bounds of the container that are to be considered
			 *			dockable and interactable.
			 */
			Rect2I getContentBounds() const;

			bool mIsLeaf;
			DockContainer* mChildren[2];
			DockContainer* mParent;
			DockManager* mManager;
			EditorWidgetContainer* mWidgets;
			HSceneObject mGUIWidgetSO;
			GUIDockSlider* mSlider;
			Rect2I mArea;
			float mSplitPosition;
			bool mIsHorizontal;

			static const UINT32 SLIDER_SIZE;
			static const UINT32 MIN_CHILD_SIZE;

		private:
			/**
			 * @brief	Updates sizes and positions of all child containers. 
			 *			Normally called when parent area changes.
			 */
			void updateChildAreas();

			/**
			 * @brief	Triggered whenever the user drags the GUI slider belonging to this container.
			 */
			void sliderDragged(const Vector2I& delta);

			/**
			 * @brief	Triggered whenever the user closes or undocks a widget belonging to this container.
			 */
			void widgetRemoved();

			/**
			 * @brief	Triggered when the maximize button in the container's title bar is clicked.
			 */
			void maximizeClicked();
		};

		/**
		 * @brief	Available dock locations for the dock manager.
		 */
		enum class DockLocation
		{
			Top,
			Bottom,
			Left,
			Right,
			None
		};
	public:
		/**
		 * @brief	Creates a new dock manager for the specified window.
		 */
		static DockManager* create(EditorWindowBase* parentWindow);

		/**
		 * @brief	Internal method. Called once every frame.
		 */
		void update();

		/**
		 * @brief	Inserts a new widget at the specified location.
		 *
		 * @param	relativeTo		Container relative to which to insert the widget. Can be null in which case
		 *							the widget is inserted at the root.
		 * @param	widgetToInsert	Widget we want to insert into the dock layout.
		 * @param	location		Location to insert the widget at, relative to "relativeTo" container. 
		 *							If "relativeTo" is null this is ignored.
		 */
		void insert(EditorWidgetContainer* relativeTo, EditorWidgetBase* widgetToInsert, DockLocation location);

		/**
		 * @brief	Returns a saved layout of all the currently docked widgets and their positions
		 *			and areas.
		 */
		DockManagerLayoutPtr getLayout() const;

		/**
		 * @brief	Sets a previously saved layout of docked widgets. This will close all currently active
		 *			widgets and open and position new ones according to the layout.
		 */
		void setLayout(const DockManagerLayoutPtr& layout);

		/**
		 * @brief	Changes the position and size of the dock manager.
		 */
		void setArea(INT32 x, INT32 y, UINT32 width, UINT32 height);

		/**
		 * @brief	Closes all docked widgets.
		 */
		void closeAll();

	private:
		DockManager(EditorWindowBase* parentWindow, const GUIDimensions& dimensions);
		~DockManager();

		/**
		 * @brief	Updates the dock overlay mesh that is displayed when user is dragging a widget
		 *			over a certain area.
		 */
		void updateDropOverlay(INT32 x, INT32 y, UINT32 width, UINT32 height);

		/**
		 * @brief	Checks is the provided point inside the provided polygon.
		 *
		 * @param	polyPoints	Points of the polygon to test against.
		 * @param	numPoints	Number of points in "polyPoints".
		 * @param	point		Point to check if it's in the polygon.
		 *
		 * @returns	True if the point is in the polygon.
		 */
		bool insidePolygon(Vector2* polyPoints, UINT32 numPoints, Vector2 point) const;

		/**
		 * @brief	Initializes the renderer used for displaying the dock overlay.
		 */
		void initializeOverlayRenderer(const SPtr<MaterialCore>& initData);

		/**
		 * @brief	Destroys the dock overlay renderer.
		 */
		void destroyOverlayRenderer(DockOverlayRenderer* core);

		/**
		 * @copydoc GUIElementBase::updateClippedBounds
		 */
		void updateClippedBounds() override;

		/**
		 * @brief	Maximizes or restored the specified container. If any container is previously
		 * 			maximized it needs to be toggled back to restored state before maximizing another.
		 */
		void toggleMaximize(DockContainer* container);

		/**
		 * @copydoc GUIElementBase::_mouseEvent
		 */
		bool _mouseEvent(const GUIMouseEvent& event) override;

		std::atomic<DockOverlayRenderer*> mCore;

		EditorWindowBase* mParentWindow;
		DockContainer mRootContainer;
		Rect2I mArea;

		HMesh mDropOverlayMesh;
		Rect2I mLastOverlayBounds;

		bool mIsMaximized;
		DockContainer* mMaximizedContainer;
		DockManagerLayoutPtr mRestoredLayout;

		DockContainer* mMouseOverContainer;
		DockLocation mHighlightedDropLoc;
		bool mShowOverlay;
		Vector2* mTopDropPolygon;
		Vector2* mBotDropPolygon;
		Vector2* mLeftDropPolygon;
		Vector2* mRightDropPolygon;
	};

	/**
	 * @brief	Handles rendering of the dock overlay, on the core thread.
	 * 			
	 * @note	Core thread only.
	 */
	class DockOverlayRenderer
	{
	public:
		DockOverlayRenderer();
		~DockOverlayRenderer();

	private:
		friend class DockManager;

		/**
		 * @brief	Initializes the object. Must be called right after construction and before any use.
		 *
		 * @param	material	Material used for drawing the dock overlay.
		 */
		void initialize(const SPtr<MaterialCore>& material);

		/**
		 * @brief	Updates the grid mesh to render.
		 * 			
		 * @param	camera		Camera to render the dock overlay to.
		 * @param	mesh		Overlay mesh to render.
		 * @param	active		Should the overlay be shown or not.
		 * @param	location	Highlighted location of the overlay.
		 */
		void updateData(const SPtr<CameraCore>& camera, const SPtr<MeshCore>& mesh, bool active,
			DockManager::DockLocation location);

		/**
		 * @brief	Callback triggered by the renderer, actually draws the dock overlay.
		 */
		void render();
		
		SPtr<CameraCore> mCamera;
		SPtr<MaterialCore> mMaterial;
		SPtr<MeshCore> mMesh;
		DockManager::DockLocation mHighlightedDropLoc;
		bool mShowOverlay;

		static const Color TINT_COLOR;
		static const Color HIGHLIGHT_COLOR;
	};
}