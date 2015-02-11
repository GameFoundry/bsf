#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	enum class DockLocation
	{
		Left, Right, Top, Bottom, Center
	};

	class DockManager : public GUIElementContainer
	{
	public:
		class DockContainer
		{
		public:
			DockContainer(DockManager* manager);
			DockContainer(DockManager* manager, DockContainer* parent);
			~DockContainer();

			void setArea(INT32 x, INT32 y, UINT32 width, UINT32 height);
			void makeLeaf(GUIWidget* widgetParent, EditorWindowBase* parentWindow);
			void makeLeaf(EditorWidgetContainer* existingContainer);
			void splitContainer(bool horizontal, bool newChildIsFirst, float splitPosition = 0.5f);
			void makeSplit(GUIWidget* widgetParent, DockManager::DockContainer* first, DockManager::DockContainer* second, bool horizontal, float splitPosition);
			void addLeft(EditorWidgetBase* widget);
			void addRight(EditorWidgetBase* widget);
			void addTop(EditorWidgetBase* widget);
			void addBottom(EditorWidgetBase* widget);

			void addWidget(EditorWidgetBase* widget);
			void addWidget(const String& name);

			void update();

			DockContainer* find(EditorWidgetContainer* widgetContainer);

			/**
			 * @brief	Searches for a DockContainer at the specified position.
			 *
			 * @param	pos	Position in the same space as DockContainer. 
			 *
			 * @return	null if it fails, else the found DockContainer at position.
			 */
			DockContainer* findAtPos(const Vector2I& pos);

			Rect2I getContentBounds() const;

			bool mIsLeaf;
			DockContainer* mChildren[2];
			DockContainer* mParent;
			DockManager* mManager;
			EditorWidgetContainer* mWidgets;
			GUIDockSlider* mSlider;
			Rect2I mArea;
			float mSplitPosition;
			bool mIsHorizontal;

			static const UINT32 SLIDER_SIZE;
			static const UINT32 MIN_CHILD_SIZE;

		private:
			void updateChildAreas();

			void sliderDragged(const Vector2I& delta);
			void widgetRemoved();
		};

		enum class DockLocation
		{
			Top,
			Bottom,
			Left,
			Right,
			None
		};
	public:
		static DockManager* create(EditorWindowBase* parentWindow);

		/**
		 * @brief	Internal method. Called once every frame.
		 */
		void update();

		void render(const Viewport* viewport, DrawList& renderQueue);
		void insert(EditorWidgetContainer* relativeTo, EditorWidgetBase* widgetToInsert, DockLocation location);

		DockManagerLayoutPtr getLayout() const;
		void setLayout(const DockManagerLayoutPtr& layout);

		void setArea(INT32 x, INT32 y, UINT32 width, UINT32 height);

		void closeAll();

	protected:
		~DockManager();

		void updateClippedBounds();

	private:
		DockManager(EditorWindowBase* parentWindow, const GUILayoutOptions& layoutOptions);

		static const Color TINT_COLOR;
		static const Color HIGHLIGHT_COLOR;

		EditorWindowBase* mParentWindow;
		DockContainer mRootContainer;
		Rect2I mArea;

		HMesh mDropOverlayMesh;
		HMaterial mDropOverlayMat;
		Rect2I mLastOverlayBounds;

		DockContainer* mMouseOverContainer;
		DockLocation mHighlightedDropLoc;
		bool mShowOverlay;
		Vector2* mTopDropPolygon;
		Vector2* mBotDropPolygon;
		Vector2* mLeftDropPolygon;
		Vector2* mRightDropPolygon;

		HEvent mRenderCallback;

		void updateDropOverlay(INT32 x, INT32 y, UINT32 width, UINT32 height);

		bool _mouseEvent(const GUIMouseEvent& event);
		bool insidePolygon(Vector2* polyPoints, UINT32 numPoints, Vector2 point) const;
	};
}