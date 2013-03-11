#pragma once

#include "CmEditorPrerequisites.h"
#include <QtWidgets/QMainWindow>
#include <boost/function.hpp>

namespace CamelotEditor
{
	class QtEditor : public QMainWindow
	{
		Q_OBJECT

	public:
		QtEditor(QWidget *parent = nullptr);
		~QtEditor();

		QtDockOverlayWidget* getDockOverlayWidget() const { return mDockOverlayWidget; }
		QWidget* getCentralWidget() const { return mCentralWidget; }

		void setProjectName(const QString& name);
		QAction* addMenuItemCallback(const QString& menuCategory, const QString& itemName, boost::function<void()> callback);
		void addMenuItemSeparator(const QString& menuCategory);

	private:
		QMenuBar* mMenuBar;
		QWidget* mCentralWidget;
		QStatusBar* mStatusBar;
		QtDockOverlayWidget* mDockOverlayWidget;

		map<QString, QMenu*>::type mMenus;

		void setupUi();
		void retranslateUi();
		void setObjectNames();

		QMenu* findOrCreateMenu(const QString& name);

		void changeEvent(QEvent* event);
		void moveEvent(QMoveEvent* event);
		void resizeEvent(QResizeEvent* event);

		bool eventFilter(QObject* object, QEvent* event);

		void openProject();
		void saveProject();
		void exitEditor();
	};
}