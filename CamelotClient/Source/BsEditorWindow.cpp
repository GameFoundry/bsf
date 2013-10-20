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
		updateSize();
		
		mWidgets->onWidgetClosed.connect(boost::bind(&EditorWindow::widgetRemoved, this));
		mWidgets->onWidgetHidden.connect(boost::bind(&EditorWindow::widgetHidden, this));
	}

	EditorWindow::~EditorWindow()
	{
		cm_delete(mWidgets);
	}

	void EditorWindow::resized()
	{
		EditorWindowBase::resized();

		updateSize();
	}

	void EditorWindow::updateSize()
	{
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

	void EditorWindow::widgetHidden()
	{
		if(mWidgets->getNumWidgets() == 1)
			hide();
	}

	EditorWindow* EditorWindow::create()
	{
		return EditorWindowManager::instance().create();
	}
}