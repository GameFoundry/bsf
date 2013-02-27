#pragma once

#include "CmEditorPrerequisites.h"
#include "CmModule.h"
#include "CmQtDockOverlayWidget.h"

class QSplitter;

namespace CamelotEditor
{
	class WindowDockManager : public Module<WindowDockManager>
	{
	public:
		WindowDockManager(QWidget* centralWidget, QtDockOverlayWidget* dockWidget);

		void windowDragged(QtEditorWindow* window, const QPoint& mousePos);
		void windowReleased(QtEditorWindow* window, const QPoint& mousePos);
		void windowClosed(QtEditorWindow* window);

		void dockWindow(QtEditorWindow* windowToDock, QtEditorWindow* dockAtWidget, WindowDragDropLocation dockAtPosition);
		void undockWindow(QtEditorWindow* windowToUndock);
	private:
		QtDockOverlayWidget* mDockOverlayWidget;
		QWidget* mCentralWidget;
		QSplitter* mCentralSplitter;
		QtEditorWindow* mLastDraggedWindow;
		QPoint mLastDragPosition;

		std::vector<QtEditorWindow*> mDockedWindows;

		QtEditorWindow* getDockedWindowAtPosition(const QPoint& globalPos);
		bool isPositionInDockArea(const QPoint& globalPos);

		WindowDragDropLocation getDropLocationAtPosition(const QWidget* widget, const QPoint& globalPos);
		std::vector<QPolygon> getDropLocations(const QWidget* widget);
	};

	WindowDockManager& gWindowDockManager();
}