#include "DbgEditorWidget2.h"
#include "BsEditorWidgetManager.h"
#include "BsGUIToggle.h"
#include "BsGUIScrollArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	DbgEditorWidget2* DbgEditorWidget2::Instance = nullptr;

	DbgEditorWidget2::DbgEditorWidget2(const ConstructPrivately& dummy, EditorWidgetContainer& parentContainer)
		:EditorWidget<DbgEditorWidget2>(HString(L"DbgEditorWidget2"), parentContainer)
	{
		GUILayout& layout = mContent->getLayout();

		GUIScrollArea* scrollArea = GUIScrollArea::create(getParentWidget());
		layout.addElement(scrollArea);

		GUILayout& scrollLayout = scrollArea->getLayout().addLayoutY();

		std::shared_ptr<GUIToggleGroup> toggleGroup = GUIToggle::createToggleGroup();

		scrollLayout.addElement(GUIToggle::create(getParentWidget(), HString(L"Test A"), toggleGroup, getParentWidget().getSkin().getStyle("Button")));
		scrollLayout.addElement(GUIToggle::create(getParentWidget(), HString(L"Test B"), toggleGroup, getParentWidget().getSkin().getStyle("Button")));
		scrollLayout.addElement(GUIToggle::create(getParentWidget(), HString(L"Test C"), toggleGroup, getParentWidget().getSkin().getStyle("Button")));
		scrollLayout.addElement(GUIToggle::create(getParentWidget(), HString(L"Test D"), toggleGroup, getParentWidget().getSkin().getStyle("Button")));
		scrollLayout.addElement(GUIToggle::create(getParentWidget(), HString(L"Test E"), toggleGroup, getParentWidget().getSkin().getStyle("Button")));
	}

	DbgEditorWidget2::~DbgEditorWidget2()
	{

	}

	DbgEditorWidget2* DbgEditorWidget2::instance()
	{
		return Instance;
	}

	DbgEditorWidget2* DbgEditorWidget2::open()
	{
		return static_cast<DbgEditorWidget2*>(EditorWidgetManager::instance().open(getTypeName()));
	}

	void DbgEditorWidget2::close()
	{
		if(Instance != nullptr)
			EditorWidgetManager::instance().close(Instance);

		Instance = nullptr; 
	}

	const String& DbgEditorWidget2::getTypeName()
	{
		static String name = "DbgEditorWidget2";
		return name;
	}
}