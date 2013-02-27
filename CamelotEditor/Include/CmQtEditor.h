#pragma once

#include "CmEditorPrerequisites.h"
#include <QtWidgets/QMainWindow>

namespace CamelotEditor
{
	class QtEditor : public QMainWindow
	{
		Q_OBJECT

	public:
		QtEditor(QWidget *parent = nullptr);
		~QtEditor();

	private:
		QMenuBar* mMenuBar;
		QToolBar* mMainToolBar;
		QWidget* mCentralWidget;
		QStatusBar* mStatusBar;
		QtDockOverlayWidget* mDockOverlayWidget;

		QMenu* mFileMenu;
		QMenu* mWindowMenu;

		void setupUi();
		void retranslateUi();
	};
}