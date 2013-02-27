#include "CmQtEditor.h"
#include "CmProjectPrefs.h"
#include "CmQtDockOverlayWidget.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtCore/QLocale>
#include <QtWidgets/QApplication>

namespace CamelotEditor
{
	QtEditor::QtEditor(QWidget *parent)
		: QMainWindow(parent)
	{
		setupUi();
	}

	QtEditor::~QtEditor()
	{

	}

	void QtEditor::setupUi()
	{
		if (objectName().isEmpty())
			setObjectName(QStringLiteral("CamelotEditorClass"));

		resize(600, 400);

		mMenuBar = new QMenuBar(this);
		mMenuBar->setObjectName(QStringLiteral("menuBar"));

		setMenuBar(mMenuBar);
		mMainToolBar = new QToolBar(this);
		mMainToolBar->setObjectName(QStringLiteral("mainToolBar"));

		addToolBar(mMainToolBar);
		mCentralWidget = new QWidget(this);
		mCentralWidget->setObjectName(QStringLiteral("centralWidget"));
		setCentralWidget(mCentralWidget);

		mStatusBar = new QStatusBar(this);
		mStatusBar->setObjectName(QStringLiteral("statusBar"));
		setStatusBar(mStatusBar);
		
		mFileMenu = new QMenu(mMenuBar);
		mFileMenu->setObjectName(QString::fromUtf8("fileMenu"));

		mWindowMenu = new QMenu(mMenuBar);
		mWindowMenu->setObjectName(QString::fromUtf8("windowMenu"));

		mMenuBar->addAction(mFileMenu->menuAction());
		mMenuBar->addAction(mWindowMenu->menuAction());

		mDockOverlayWidget = new QtDockOverlayWidget(this);

		retranslateUi();

		QMetaObject::connectSlotsByName(this);
	}

	void QtEditor::retranslateUi()
	{
		QString title = tr("Camelot Editor") + " - " + gProjectPrefs().getProjectName();

		setWindowTitle(title);
		mFileMenu->setTitle(tr("File"));
		mWindowMenu->setTitle(tr("Windows"));
	}
}

