#include "CmQtEditor.h"
#include "CmQtDockOverlayWidget.h"
#include "CmEditorPrefs.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtCore/QLocale>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QMoveEvent>
#include <QtGui/QResizeEvent>
#include <boost/bind.hpp>
#include "CmException.h"

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
		WindowLayoutDesc desc = gEditorPrefs().getMainWindowLayout();

		if(desc.maximized)
		{
			setGeometry(desc.left, desc.top, desc.width, desc.height);
			setWindowState(Qt::WindowMaximized);
		}
		else
		{
			setGeometry(desc.left, desc.top, desc.width, desc.height);
		}

		mMenuBar = new QMenuBar(this);
		setMenuBar(mMenuBar);

		mCentralWidget = new QWidget(this);
		setCentralWidget(mCentralWidget);

		mDockOverlayWidget = new QtDockOverlayWidget(this);

		mStatusBar = new QStatusBar(this);
		setStatusBar(mStatusBar);

		setContextMenuPolicy(Qt::NoContextMenu);
		
		addMenuItemCallback("File", "Open project", boost::bind(&QtEditor::openProject, this));
		addMenuItemCallback("File", "Save project", boost::bind(&QtEditor::saveProject, this));
		addMenuItemSeparator("File");
		addMenuItemCallback("File", "Exit", boost::bind(&QtEditor::exitEditor, this));

		retranslateUi();
		setObjectNames();

		QMetaObject::connectSlotsByName(this);
	}

	void QtEditor::retranslateUi()
	{
		setProjectName("No project");
	}

	void QtEditor::setObjectNames()
	{
		mMenuBar->setObjectName(QStringLiteral("MenuBar"));
		mCentralWidget->setObjectName(QStringLiteral("CentralWidget"));
		mStatusBar->setObjectName(QStringLiteral("StatusBar"));
		mDockOverlayWidget->setObjectName(QStringLiteral("DockOverlayWidget"));
	}

	void QtEditor::setProjectName(const QString& name)
	{
		QString title = tr("Camelot Editor") + " - " + name;

		setWindowTitle(title);
	}

	QAction* QtEditor::addMenuItemCallback(const QString& menuCategory, const QString& itemName, boost::function<void()> callback)
	{
		QMenu* menu = findOrCreateMenu(menuCategory);

		QAction* newAction = menu->addAction(itemName);
		connect(newAction, &QAction::triggered, callback);

		return newAction;
	}

	void QtEditor::addMenuItemSeparator(const QString& menuCategory)
	{
		QMenu* menu = findOrCreateMenu(menuCategory);

		menu->addSeparator();
	}

	QMenu* QtEditor::findOrCreateMenu(const QString& name)
	{
		auto iterFind = mMenus.find(name);
		QMenu* menu = nullptr;

		if(iterFind == mMenus.end())
		{
			menu = new QMenu(mMenuBar);
			menu->setObjectName(name);
			menu->setTitle(name);
			mMenuBar->addAction(menu->menuAction());

			mMenus[name] = menu;

			return menu;
		}
		else
			return iterFind->second;
	}

	void QtEditor::changeEvent(QEvent* event)
	{
		if(event->type() == QEvent::WindowStateChange) 
		{
			WindowLayoutDesc desc = gEditorPrefs().getMainWindowLayout();
			if(isMaximized()) 
				desc.maximized = true;
			else 
				desc.maximized = false;

			desc.screenIdx = QApplication::desktop()->screenNumber(this);
			gEditorPrefs().setMainWindowLayout(desc);
		}

		event->accept();
	}

	void QtEditor::moveEvent(QMoveEvent* event)
	{
		WindowLayoutDesc desc = gEditorPrefs().getMainWindowLayout();
		desc.left = event->pos().x();
		desc.top = event->pos().y();

		gEditorPrefs().setMainWindowLayout(desc);

		QWidget::moveEvent(event);
	}

	void QtEditor::resizeEvent(QResizeEvent* event)
	{
		WindowLayoutDesc desc = gEditorPrefs().getMainWindowLayout();
		desc.width = event->size().width();
		desc.height = event->size().height();

		gEditorPrefs().setMainWindowLayout(desc);

		mDockOverlayWidget->resize(event->size());
		QWidget::resizeEvent(event);
	}

	bool QtEditor::eventFilter(QObject* object, QEvent* event)
	{


		return false;
	}

	void QtEditor::openProject()
	{
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}

	void QtEditor::saveProject()
	{
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}

	void QtEditor::exitEditor()
	{
		exit(1);
	}
}

