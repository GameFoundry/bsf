#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEditor
{
	enum class DockLocation
	{
		Left, Right, Top, Bottom, Center
	};

	class DockManager
	{
		class DockContainer
		{
		public:
			DockContainer();
			~DockContainer();

			void setArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);
			void makeLeaf(BS::GUIWidget* widgetParent, EditorWidget* widget);
			void addLeft(BS::GUIWidget* widgetParent, EditorWidget* widget);
			void addRight(BS::GUIWidget* widgetParent, EditorWidget* widget);
			void addTop(BS::GUIWidget* widgetParent, EditorWidget* widget);
			void addBottom(BS::GUIWidget* widgetParent, EditorWidget* widget);

			DockContainer* find(EditorWidgetContainer* widgetContainer);

			/**
			 * @brief	Searches for a DockContainer at the specified position.
			 *
			 * @param	pos	Position in the same space as DockContainer. 
			 *
			 * @return	null if it fails, else the found DockContainer at position.
			 */
			DockContainer* findAtPos(const CM::Int2& pos);

			bool mIsLeaf;
			DockContainer* mChildren[2];
			EditorWidgetContainer* mWidgets;
			CM::INT32 mX, mY;
			CM::UINT32 mWidth, mHeight;
			float mSplitPosition;
			bool mIsHorizontal;

			static const CM::UINT32 SliderSize;

		private:
			void splitContainer(BS::GUIWidget* widgetParent, EditorWidget* widget, bool horizontal, bool newChildIsFirst);
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
		DockManager(BS::GUIWidget* parent);
		~DockManager();

		void render(const CM::Viewport* viewport, CM::RenderQueue& renderQueue);
		void insert(EditorWidgetContainer* relativeTo, EditorWidget* widgetToInsert, DockLocation location);

		void setArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);

	private:
		static const CM::Color TINT_COLOR;
		static const CM::Color HIGHLIGHT_COLOR;

		BS::GUIWidget* mParent;
		DockContainer mRootContainer;

		CM::HMesh mDropOverlayMesh;
		CM::HMaterial mDropOverlayMat;

		DockContainer* mMouseOverContainer;
		DockLocation mHighlightedDropLoc;
		CM::Vector2* mTopDropPolygon;
		CM::Vector2* mBotDropPolygon;
		CM::Vector2* mLeftDropPolygon;
		CM::Vector2* mRightDropPolygon;

		void updateDropOverlay(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);

		void onGUIMouseEvent(BS::GUIWidget* widget, BS::GUIElement* element, const BS::GUIMouseEvent& event);
		bool insidePolygon(CM::Vector2* polyPoints, CM::UINT32 numPoints, CM::Vector2 point) const;
	};
}