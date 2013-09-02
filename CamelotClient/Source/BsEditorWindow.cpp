#include "BsEditorWindow.h"
#include "BsEditorWidgetContainer.h"
#include "BsEditorWindowManager.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWindow::EditorWindow()
		:EditorWindowBase(), mWidgets(cm_new<EditorWidgetContainer>(mGUI.get()))
	{
		mWidgets->onWidgetClosed.connect(boost::bind(&EditorWindow::widgetRemoved, this));
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

	void EditorWindow::widgetRemoved()
	{
		if(mWidgets->getNumWidgets() == 0)
			close();
	}

	EditorWindow* EditorWindow::create()
	{
		return EditorWindowManager::instance().create();
	}
}