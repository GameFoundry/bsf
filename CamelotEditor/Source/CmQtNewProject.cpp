#include "CmQtNewProject.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace CamelotEditor
{
	QtNewProject::QtNewProject(QWidget *parent)
		:QDialog(parent)
	{
		setupUi();
	}

	QtNewProject::~QtNewProject()
	{ }

	QTextEdit* mTxtProjectName;
	QPushButton* mBtnCreate;
	QTextEdit* mTxtProjectPath;
	QPushButton* mBtnBrowse;

	void QtNewProject::setupUi()
	{
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
		setFixedSize(400, 170);

		mTxtProjectName = new QTextEdit(this);
		mTxtProjectPath = new QTextEdit(this);

		mBtnCreate = new QPushButton(this);
		mBtnBrowse = new QPushButton(this);

		QWidget* nameWidget = new QWidget(this);

		QHBoxLayout* pathLayout = new QHBoxLayout(this); 
		pathLayout->setMargin(0);
		pathLayout->addWidget(mTxtProjectPath);
		pathLayout->addWidget(mBtnBrowse);

		nameWidget->setLayout(pathLayout);

		QVBoxLayout* centralLayout = new QVBoxLayout(this);
		centralLayout->setMargin(0);
		centralLayout->addWidget(mTxtProjectName);
		centralLayout->addWidget(nameWidget);
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
		mBtnCreate->setText(tr("Create"));
		mBtnBrowse->setText(tr("Browse"));
	}

	void QtNewProject::createProject()
	{

	}

	void QtNewProject::browseProjectPath()
	{

	}
}