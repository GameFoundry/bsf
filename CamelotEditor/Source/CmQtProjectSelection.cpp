#include "CmQtProjectSelection.h"
#include "CmEditorPrefs.h"
#include "CmEditorApplication.h"
#include "CmQtNewProject.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

namespace CamelotEditor
{
	QtProjectSelection::QtProjectSelection(QWidget *parent)
		:QDialog(parent)
	{
		setupUi();
	}

	QtProjectSelection::~QtProjectSelection()
	{

	}

	void QtProjectSelection::setupUi()
	{
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
		setFixedSize(400, 170);

		mListWidget = new QListWidget(this);

		QWidget* buttonsWidget = new QWidget(this);
		mBtnNew = new QPushButton(buttonsWidget);
		mBtnOpen = new QPushButton(buttonsWidget);
		mBtnBrowse = new QPushButton(buttonsWidget);

		QHBoxLayout* buttonLayout = new QHBoxLayout(this);
		buttonLayout->setMargin(0);
		buttonLayout->addWidget(mBtnNew);
		buttonLayout->addWidget(mBtnBrowse);
		buttonLayout->addWidget(mBtnOpen);
		buttonsWidget->setLayout(buttonLayout);
		
		QVBoxLayout* centralLayout = new QVBoxLayout(this);
		centralLayout->setMargin(0);
		centralLayout->addWidget(mListWidget);
		centralLayout->addWidget(buttonsWidget);
		setLayout(centralLayout);

		reloadProjectList();

		retranslateUi();
		setupSignals();
	}

	void QtProjectSelection::retranslateUi()
	{
		setWindowTitle(tr("Project selection"));
		mBtnNew->setText(tr("New"));
		mBtnBrowse->setText(tr("Browse"));
		mBtnOpen->setText(tr("Open"));
	}

	void QtProjectSelection::setupSignals()
	{
		connect(mBtnNew, SIGNAL(clicked()), this, SLOT(newProject()));
		connect(mBtnBrowse, SIGNAL(clicked()), this, SLOT(browseProject()));
		connect(mBtnOpen, SIGNAL(clicked()), this, SLOT(openSelectedProject()));
	}

	void QtProjectSelection::reloadProjectList()
	{
		mListWidget->clear();

		for(int i = 0; i < gEditorPrefs().getNumRecentlyUsedProjects(); i++)
			mListWidget->insertItem(i, gEditorPrefs().getRecentlyUsedProjectPath(i));
	}

	void QtProjectSelection::newProject()
	{
		QtNewProject newProject;
		if(newProject.exec() ==  QDialog::Accepted)
		{
			openProject(newProject.getProjectPath());
		}
	}

	void QtProjectSelection::browseProject()
	{
		QFileDialog dialog(this, tr("Select project directory"));
		dialog.setFileMode(QFileDialog::Directory);
		dialog.setOption(QFileDialog::ShowDirsOnly, true);

		QDir lastUsedDir = gEditorPrefs().getLastUsedProjectDirectory();
		if(lastUsedDir.exists())
			dialog.setDirectory(lastUsedDir);

		if(dialog.exec() == QDialog::Accepted)
		{
			QStringList fileNames = dialog.selectedFiles();

			if(fileNames.size() > 0)
			{
				openProject(fileNames[0]);
			}
		}
	}

	void QtProjectSelection::openSelectedProject()
	{
		QListWidgetItem* item = mListWidget->currentItem();

		if(item == nullptr)
		{
			QMessageBox msgBox;
			msgBox.setWindowTitle("Error");
			msgBox.setText("Please select a project before continuing.");
			msgBox.exec();

			return;
		}

		openProject(item->text());
	}

	void QtProjectSelection::openProject(const QString& path)
	{
		if(gEditorApp().isValidProject(path))
		{
			addRecentlyUsedProject(path);
			onProjectSelected(path);
			accept();
		}
		else
		{		
			removeRecentlyUsedProject(path);
			reloadProjectList();

			QMessageBox msgBox;
			msgBox.setWindowTitle("Error");
			msgBox.setText("Selected directory doesn't contain a valid Camelot project");
			msgBox.exec();

			return;
		}
	}

	void QtProjectSelection::addRecentlyUsedProject(const QString& path)
	{
		bool found = false;
		for(int i = 0; i < gEditorPrefs().getNumRecentlyUsedProjects(); i++)
		{
			if(path == gEditorPrefs().getRecentlyUsedProjectPath(i))
			{
				found = true;
				break;
			}
		}

		gEditorPrefs().addRecentlyUsedProjectPath(path);
	}

	void QtProjectSelection::removeRecentlyUsedProject(const QString& path)
	{
		for(int i = 0; i < gEditorPrefs().getNumRecentlyUsedProjects(); i++)
		{
			if(path == gEditorPrefs().getRecentlyUsedProjectPath(i))
			{
				gEditorPrefs().removeRecentlyUsedProjectPath(i);
				break;
			}
		}
	}
}