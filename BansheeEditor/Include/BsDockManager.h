#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "CmRectI.h"

namespace BansheeEditor
{
	enum class DockLocation
	{
		Left, Right, Top, Bottom, Center
	};

	class DockManager : public BS::GUIElementContainer
	{
	public:
		class DockContainer
		{
		public:
			DockContainer();
			DockContainer(DockContainer* parent);
			~DockContainer();

			void setArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);
			void makeLeaf(BS::GUIWidget* widgetParent, CM::RenderWindow* parentWindow);
			void makeLeaf(EditorWidgetContainer* existingContainer);
			void splitContainer(BS::GUIWidget* widgetParent, CM::RenderWindow* parentWindow, bool horizontal, bool newChildIsFirst, float splitPosition = 0.5f);
			void addLeft(BS::GUIWidget* widgetParent, CM::RenderWindow* parentWindow, EditorWidgetBase* widget);
			void addRight(BS::GUIWidget* widgetParent, CM::RenderWindow* parentWindow, EditorWidgetBase* widget);
			void addTop(BS::GUIWidget* widgetParent, CM::RenderWindow* parentWindow, EditorWidgetBase* widget);
			void addBottom(BS::GUIWidget* widgetParent, CM::RenderWindow* parentWindow, EditorWidgetBase* widget);

			void addWidget(EditorWidgetBase* widget);
			void addWidget(const CM::String& name);

			DockContainer* find(EditorWidgetContainer* widgetContainer);

			/**
			 * @brief	Searches for a DockContainer at the specified position.
			 *
			 * @param	pos	Position in the same space as DockContainer. 
			 *
			 * @return	null if it fails, else the found DockContainer at position.
			 */
			DockContainer* findAtPos(const CM::Vector2I& pos);

			CM::RectI getContentBounds() const;

			bool mIsLeaf;
			DockContainer* mChildren[2];
			DockContainer* mParent;
			EditorWidgetContainer* mWidgets;
			GUIDockSlider* mSlider;
			CM::RectI mArea;
			float mSplitPosition;
			bool mIsHorizontal;

			static const CM::UINT32 SLIDER_SIZE;
			static const CM::UINT32 MIN_CHILD_SIZE;

		private:
			void updateChildAreas();

			void sliderDragged(const CM::Vector2I& delta);
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
		static DockManager* create(CM::RenderWindow* parentWindow);

		/**
		 * @brief	Internal method. Called once every frame.
		 */
		void update();

		void render(const CM::Viewport* viewport, CM::RenderQueue& renderQueue);
		void insert(EditorWidgetContainer* relativeTo, EditorWidgetBase* widgetToInsert, DockLocation location);

		DockManagerLayoutPtr getLayout() const;
		void setLayout(const DockManagerLayoutPtr& layout);

		void setArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);

	protected:
		~DockManager();

		void updateClippedBounds();

	private:
		DockManager(CM::RenderWindow* parentWindow, const BS::GUILayoutOptions& layoutOptions);

		static const CM::Color TINT_COLOR;
		static const CM::Color HIGHLIGHT_COLOR;

		CM::RenderWindow* mParentWindow;
		DockContainer mRootContainer;
		CM::RectI mArea;

		CM::HMesh mDropOverlayMesh;
		CM::HMaterial mDropOverlayMat;
		CM::RectI mLastOverlayBounds;

		DockContainer* mMouseOverContainer;
		DockLocation mHighlightedDropLoc;
		bool mShowOverlay;
		CM::Vector2* mTopDropPolygon;
		CM::Vector2* mBotDropPolygon;
		CM::Vector2* mLeftDropPolygon;
		CM::Vector2* mRightDropPolygon;

		void updateDropOverlay(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);

		bool mouseEvent(const BS::GUIMouseEvent& event);
		bool insidePolygon(CM::Vector2* polyPoints, CM::UINT32 numPoints, CM::Vector2 point) const;
	};
}