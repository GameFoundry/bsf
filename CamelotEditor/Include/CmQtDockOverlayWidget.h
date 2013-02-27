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

	private:
		WindowDragDropLocation mHighlightedDropLocation;
		bool mDropOverlayEnabled;
		vector<QPolygon>::type mDragLocations;
		QPoint mOverlayOffset;

		void paintEvent(QPaintEvent* event);

		void drawDragLocations(const std::vector<QPolygon>& dragLocations, WindowDragDropLocation highlightedLocation);
	};
}