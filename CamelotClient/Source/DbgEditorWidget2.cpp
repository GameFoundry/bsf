#include "DbgEditorWidget2.h"
#include "BsEditorWindow.h"
#include "BsGUIToggle.h"
#include "BsGUIScrollArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsEditorWindow.h"
#include "BsEditorWidgetContainer.h"
#include "BsEngineGUI.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	DbgEditorWidget2* DbgEditorWidget2::Instance = nullptr;

	DbgEditorWidget2::DbgEditorWidget2()
		:EditorWidget(L"DbgEditorWidget2")
	{

	}

	DbgEditorWidget2::~DbgEditorWidget2()
	{

	}

	void DbgEditorWidget2::initialize()
	{
		GUILayout& layout = mContent->getLayout();

		GUIScrollArea* scrollArea = GUIScrollArea::create(getParentWidget());
		layout.addElement(scrollArea);

		GUILayout& scrollLayout = scrollArea->getLayout().addLayoutY();

		std::shared_ptr<GUIToggleGroup> toggleGroup = GUIToggle::createToggleGroup();

		scrollLayout.addElement(GUIToggle::create(getParentWidget(), L"Test A", toggleGroup, EngineGUI::instance().getSkin().getStyle("Button")));
		scrollLayout.addElement(GUIToggle::create(getParentWidget(), L"Test B", toggleGroup, EngineGUI::instance().getSkin().getStyle("Button")));
		scrollLayout.addElement(GUIToggle::create(getParentWidget(), L"Test C", toggleGroup, EngineGUI::instance().getSkin().getStyle("Button")));
		scrollLayout.addElement(GUIToggle::create(getParentWidget(), L"Test D", toggleGroup, EngineGUI::instance().getSkin().getStyle("Button")));
		scrollLayout.addElement(GUIToggle::create(getParentWidget(), L"Test E", toggleGroup, EngineGUI::instance().getSkin().getStyle("Button")));
	}

	DbgEditorWidget2* DbgEditorWidget2::instance()
	{
		return Instance;
	}

	DbgEditorWidget2* DbgEditorWidget2::open()
	{
		if(Instance != nullptr)
			return Instance;

		EditorWindow* newWindow = EditorWindow::create();

		DbgEditorWidget2* newWidget = new (cm_alloc<DbgEditorWidget2>()) DbgEditorWidget2();
		newWindow->getWidgets().add(*newWidget);
		newWidget->initialize();
		Instance = newWidget;

		return newWidget;
	}

	void DbgEditorWidget2::close()
	{
		if(Instance != nullptr)
		{
			Instance->mParent->_notifyWidgetDestroyed(Instance);

			EditorWidget::destroy(Instance);
		}

		Instance = nullptr; 
	}
}