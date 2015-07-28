#include "BsMainEditorWindow.h"
#include "BsEditorWindowManager.h"
#include "BsDockManager.h"
#include "BsGUIMenuBar.h"
#include "BsGUIWidget.h"
#include "BsGUIPanel.h"
#include "BsGUIStatusBar.h"
#include "BsEditorTestSuite.h"
#include "BsTestOutput.h"

namespace BansheeEngine
{
	MainEditorWindow* MainEditorWindow::create(const RenderWindowPtr& renderWindow)
	{
		return EditorWindowManager::instance().createMain(renderWindow);
	}

	MainEditorWindow::MainEditorWindow(const RenderWindowPtr& renderWindow)
		:EditorWindowBase(renderWindow), mDockManager(nullptr), 
		mMenuBar(bs_new<GUIMenuBar>(mGUI.get(), mRenderWindow.get()))
	{
		mDockManager = DockManager::create(this);
		mGUI->getPanel()->addElement(mDockManager);
		mStatusBar = GUIStatusBar::create();
		mGUI->getPanel()->addElement(mStatusBar);

		updateAreas();

		mMenuBar->addMenuItem(L"File/New project", nullptr, 100);
		mMenuBar->addMenuItem(L"File/Open project", nullptr, 100);
		mMenuBar->addSeparator(L"File", 99);
		mMenuBar->addMenuItem(L"File/Exit", nullptr, 98);

		TestSuitePtr testSuite = TestSuite::create<EditorTestSuite>();
		testSuite->run(ExceptionTestOutput());
	}

	MainEditorWindow::~MainEditorWindow()
	{
		mDockManager->closeAll();
		GUIElement::destroy(mDockManager);
		GUIElement::destroy(mStatusBar);
		bs_delete(mMenuBar);
	}

	void MainEditorWindow::resized()
	{
		EditorWindowBase::resized();

		updateAreas();
	}

	void MainEditorWindow::updateAreas()
	{
		UINT32 widgetWidth = (UINT32)std::max(0, (INT32)getWidth() - 2);
		UINT32 widgetHeight = (UINT32)std::max(0, (INT32)getHeight() - 2);

		UINT32 menuBarHeight = 15;
		mMenuBar->setArea(1, 1, widgetWidth, menuBarHeight);

		UINT32 statusBarHeight = 20;
		UINT32 dockHeight = (UINT32)std::max(0, (INT32)widgetHeight - (INT32)(menuBarHeight + statusBarHeight));
		mDockManager->setArea(1, menuBarHeight + 1, widgetWidth, dockHeight);

		mStatusBar->setPosition(1, 1 + menuBarHeight + dockHeight);
		mStatusBar->setWidth(widgetWidth);
		mStatusBar->setHeight(statusBarHeight);
	}

	void MainEditorWindow::update()
	{
		mDockManager->update();
	}
}