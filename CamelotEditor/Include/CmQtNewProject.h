#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

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

		QString getProjectPath() const;
	public:
		boost::signal<void(const QString&)> onProjectPathSelected;

	private:
		QLabel* mLblProjectName;
		QLineEdit* mTxtProjectName;
		QPushButton* mBtnCreate;
		QLabel* mLblProjectPath;
		QLineEdit* mTxtProjectPath;
		QPushButton* mBtnBrowse;

		void setupUi();
		void setupSignals();
		void retranslateUi();
		void setObjectNames();

		private Q_SLOTS:
		void createProject();
		void browseProjectPath();
	};
}