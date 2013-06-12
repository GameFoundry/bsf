#pragma once

#include "CmEditorPrerequisites.h"
#include <QtWidgets/QWidget>

namespace CamelotEditor
{
	enum WindowDragDropLocation
	{
		CM_WINDROP_LEFT,
		CM_WINDROP_RIGHT,
		CM_WINDROP_TOP,
		CM_WINDROP_BOTTOM,
		CM_WINDROP_CENTER,
		CM_WINDROP_NONE
	};

	class QtDockOverlayWidget : public QWidget
	{
		Q_OBJECT
	public:
		QtDockOverlayWidget(QWidget* parent);
		virtual ~QtDockOverlayWidget() { }

		void highlightDropLocation(WindowDragDropLocation dropLocation);

		void enableDropOverlay(std::vector<QPolygon> dragLocations, const QPoint& offset);
		void disableDropOverlay();

		void highlightTabDropLocation(INT32 dropLocation);

		void enableTabDropOverlay(std::vector<QPolygon> dragLocations, const QPoint& offset);
		void disableTabDropOverlay();

	private:
		WindowDragDropLocation mHighlightedDropLocation;
		bool mDropOverlayEnabled;
		Vector<QPolygon>::type mDragLocations;
		QPoint mOverlayOffset;

		INT32 mHighlightedTabDropLocation;
		bool mTabDropOverlayEnabled;
		Vector<QPolygon>::type mTabDropLocations;
		QPoint mTabOverlayOffset;

		void paintEvent(QPaintEvent* event);

		void drawDragLocations(const std::vector<QPolygon>& dragLocations, WindowDragDropLocation highlightedLocation);
		void drawTabDropLocations(const std::vector<QPolygon>& dropLocations, INT32 highlightedLocation);
	};
}