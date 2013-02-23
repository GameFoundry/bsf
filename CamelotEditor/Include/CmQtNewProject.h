#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>

#ifndef Q_MOC_RUN // Qt MOC doesn't handle some of the macros inside this header
#include <boost/signal.hpp>
#endif

namespace CamelotEditor
{
	class QtNewProject : public QDialog
	{
		Q_OBJECT

	public:
		QtNewProject(QWidget *parent = nullptr);
		~QtNewProject();
	public:
		boost::signal<void(const QString&)> onProjectPathSelected;

	private:
		QTextEdit* mTxtProjectName;
		QPushButton* mBtnCreate;
		QTextEdit* mTxtProjectPath;
		QPushButton* mBtnBrowse;

		void setupUi();
		void setupSignals();
		void retranslateUi();

		private Q_SLOTS:
		void createProject();
		void browseProjectPath();
	};
}