#include "CmQtNewProject.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include "CmFileSystem.h"
#include "CmEditorPrefs.h"
#include "CmEditorApplication.h"

namespace CamelotEditor
{
	QtNewProject::QtNewProject(QWidget *parent)
		:QDialog(parent)
	{
		setupUi();
	}

	QtNewProject::~QtNewProject()
	{ }

	void QtNewProject::setupUi()
	{
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
		setFixedSize(400, 170);

		/************************************************************************/
		/* 							Create controls                      		*/
		/************************************************************************/

		mLblProjectName = new QLabel(this);
		mLblProjectPath = new QLabel(this);

		mTxtProjectName = new QLineEdit(this);
		mTxtProjectPath = new QLineEdit(this);

		mBtnCreate = new QPushButton(this);
		mBtnBrowse = new QPushButton(this);

		/************************************************************************/
		/* 								Name layout                      		*/
		/************************************************************************/

		QHBoxLayout* nameLayout = new QHBoxLayout(this);
		nameLayout->setMargin(0);
		nameLayout->addWidget(mLblProjectName);
		nameLayout->addWidget(mTxtProjectName);

		QWidget* nameWidget = new QWidget(this);
		nameWidget->setLayout(nameLayout);
		
		/************************************************************************/
		/* 								Path layout                      		*/
		/************************************************************************/

		QHBoxLayout* pathLayout = new QHBoxLayout(this); 
		pathLayout->setMargin(0);
		pathLayout->addWidget(mLblProjectPath);
		pathLayout->addWidget(mTxtProjectPath);
		pathLayout->addWidget(mBtnBrowse);

		QWidget* pathWidget = new QWidget(this);
		pathWidget->setLayout(pathLayout);

		/************************************************************************/
		/* 								Central layout                      	*/
		/************************************************************************/

		QVBoxLayout* centralLayout = new QVBoxLayout(this);
		centralLayout->setMargin(0);
		centralLayout->addWidget(nameWidget);
		centralLayout->addWidget(pathWidget);
		centralLayout->addWidget(mBtnCreate);

		setLayout(centralLayout);

		retranslateUi();
		setupSignals();
	}

	void QtNewProject::setupSignals()
	{
		connect(mBtnCreate, SIGNAL(clicked()), this, SLOT(createProject()));
		connect(mBtnBrowse, SIGNAL(clicked()), this, SLOT(browseProjectPath()));
	}

	void QtNewProject::retranslateUi()
	{
		setWindowTitle(tr("New project"));
		mLblProjectName->setText(tr("Name:"));
		mLblProjectPath->setText(tr("Directory:"));

		mBtnCreate->setText(tr("Create"));
		mBtnBrowse->setText(tr("Browse"));

		QDir lastUsedDir = gEditorPrefs().getLastUsedProjectDirectory();
		if(lastUsedDir.exists())
			mTxtProjectPath->setText(lastUsedDir.absolutePath());
		else
			mTxtProjectPath->setText(QString::fromStdString(FileSystem::getCurrentPath()));
	}

	void QtNewProject::createProject()
	{
		QString name = mTxtProjectName->text();
		if(!gEditorApp().isValidProjectName(name))
		{
			QMessageBox msgBox;
			msgBox.setText("Error");
			msgBox.setInformativeText("Provided project name is not valid. Please use only alphanumeric characters.");
			msgBox.exec();

			return;
		}

		QString projectDirPath = mTxtProjectPath->text();

		QString fullPath = gEditorApp().getFullProjectPath(projectDirPath, name);
		if(gEditorApp().projectExists(fullPath))
		{
			QMessageBox msgBox;
			msgBox.setText("Warning");
			msgBox.setInformativeText("There is already a project at the specified location. Do you want to overwrite?");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			
			int ret = msgBox.exec();

			if(ret == QMessageBox::Yes)
				gEditorApp().deleteProject(fullPath);
			else
				return;
		}

		QDir dir(projectDirPath);
		if(!dir.mkpath(projectDirPath))
		{
			QMessageBox msgBox;
			msgBox.setText("Error");
			msgBox.setInformativeText("Failed to create project directory.");

			return;
		}

		gEditorPrefs().setLastUsedProjectDirectory(projectDirPath);

		gEditorApp().createProject(projectDirPath, name);
		gEditorApp().loadProject(fullPath);
		accept();
	}

	void QtNewProject::browseProjectPath()
	{
		QFileDialog dialog(this, tr("Select project directory"));
		dialog.setFileMode(QFileDialog::Directory);
		dialog.setOption(QFileDialog::ShowDirsOnly, true);

		QDir lastUsedDir = gEditorPrefs().getLastUsedProjectDirectory();
		if(lastUsedDir.exists())
			dialog.setDirectory(lastUsedDir);

		if(dialog.exec())
		{
			QStringList fileNames = dialog.selectedFiles();

			if(fileNames.size() > 0)
			{
				QString dirPath = QString::fromStdString(FileSystem::getDirectoryPath(fileNames[0].toStdString()));
				mTxtProjectPath->setText(dirPath);

				gEditorPrefs().setLastUsedProjectDirectory(dirPath);
			}
		}
	}

	QString QtNewProject::getProjectPath() const
	{
		QString name = mTxtProjectName->text();
		QString projectDirPath = mTxtProjectPath->text();

		return gEditorApp().getFullProjectPath(projectDirPath, name);
	}
}