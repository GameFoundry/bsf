#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>

#ifndef Q_MOC_RUN // Qt MOC doesn't handle some of the macros inside this header
#include <boost/signal.hpp>
#endif

namespace CamelotEditor
{
	class QtProjectSelection : public QDialog
	{
		Q_OBJECT

	public:
		QtProjectSelection(QWidget *parent = nullptr);
		~QtProjectSelection();
	public:
		boost::signal<void(const QString&)> onProjectSelected;

	private:
		QListWidget* mListRecentProjects;
		QPushButton* mBtnOpen;
		QPushButton* mBtnNew;
		QPushButton* mBtnOpenOther;

		void setupUi();
		void setupSignals();
		void retranslateUi();
		void setObjectNames();

		void openProject(const QString& path);
		void addRecentlyUsedProject(const QString& path);
		void removeRecentlyUsedProject(const QString& path);

		void reloadProjectList();

	private Q_SLOTS:
		void newProject();
		void browseProject();
		void openSelectedProject();
	};
}