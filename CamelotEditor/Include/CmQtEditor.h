#pragma once

#include <QtWidgets/QMainWindow>

namespace CamelotEditor
{
	class QtEditor : public QMainWindow
	{
		Q_OBJECT

	public:
		QtEditor(QWidget *parent = 0);
		~QtEditor();

	private:
		QMenuBar* mMenuBar;
		QToolBar* mMainToolBar;
		QWidget* mCentralWidget;
		QStatusBar* mStatusBar;

		QMenu* mFileMenu;
		QMenu* mWindowMenu;

		void setupUi();
		void retranslateUi();
	};
}