#pragma once

#include "CmEditorPrerequisites.h"
#include "CmModule.h"
#include "CmQtDockOverlayWidget.h"

namespace CamelotEditor
{
	class WindowDockManager : public Module<WindowDockManager>
	{
	public:
		WindowDockManager(QWidget* centralWidget, QtDockOverlayWidget* dockWidget);

		void windowDragged(QtEditorWindow* window, const QPoint& mousePos);
		void windowReleased(QtEditorWindow* window, const QPoint& mousePos);
		void windowClosed(QtEditorWindow* window);

	private:
		QtDockOverlayWidget* mDockOverlayWidget;
		QWidget* mCentralWidget;
		QSplitter* mCentralSplitter;
		QtEditorWindow* mLastDraggedWindow;
		QPoint mLastDragPosition;

		std::vector<QtEditorWindow*> mDockedWindows;

		void dockWindow(QtEditorWindow* windowToDock, QtEditorWindow* dockAtWidget, WindowDragDropLocation dockAtPosition);
		void undockWindow(QtEditorWindow* windowToUndock);

		QtEditorWindow* getDockedWindowAtPosition(const QPoint& globalPos);
		bool isPositionInDockArea(const QPoint& globalPos);

		WindowDragDropLocation getDropLocationAtPosition(const QWidget* widget, const QPoint& globalPos);
		std::vector<QPolygon> getDropLocations(const QWidget* widget);
	};

	WindowDockManager& gWindowDockManager();
}