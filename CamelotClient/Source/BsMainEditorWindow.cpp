#include "BsMainEditorWindow.h"
#include "BsDockManager.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	MainEditorWindow::MainEditorWindow(CM::RenderWindowPtr renderWindow)
		:EditorWindowBase(renderWindow), mDockManager(cm_new<DockManager>(mGUI.get()))
	{

	}

	MainEditorWindow::~MainEditorWindow()
	{
		cm_delete(mDockManager);
	}

	void MainEditorWindow::movedOrResized()
	{
		EditorWindowBase::movedOrResized();

		UINT32 widgetWidth = (UINT32)std::max(0, (INT32)getWidth() - 2);
		UINT32 widgetHeight = (UINT32)std::max(0, (INT32)getHeight() - 2);

		mDockManager->setArea(1, 1, widgetWidth, widgetHeight);
	}
}