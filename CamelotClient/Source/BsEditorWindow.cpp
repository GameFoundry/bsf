#include "BsEditorWindow.h"
#include "BsEditorWidgetContainer.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWindow::EditorWindow()
		:mWidgets(cm_new<EditorWidgetContainer>(mGUI.get()))
	{
		
	}

	EditorWindow::~EditorWindow()
	{
		cm_delete(mWidgets);
	}

	void EditorWindow::movedOrResized()
	{
		EditorWindowBase::movedOrResized();

		mWidgets->setPosition(1, 1);

		UINT32 widgetWidth = (UINT32)std::max(0, (INT32)getWidth() - 2);
		UINT32 widgetHeight = (UINT32)std::max(0, (INT32)getHeight() - 2);

		mWidgets->setSize(widgetWidth, widgetHeight);
	}

	EditorWindow& EditorWindow::create()
	{
		EditorWindow* newWindow = new (cm_alloc<EditorWindow>()) EditorWindow();
		newWindow->initialize();

		return *newWindow;
	}
}