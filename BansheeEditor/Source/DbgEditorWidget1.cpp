#include "DbgEditorWidget1.h"
#include "BsEditorWindow.h"
#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUIInputBox.h"
#include "BsGUIScrollArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILayoutY.h"
#include "BsGUIPanel.h"
#include "BsEditorWidgetManager.h"
#include "BsGUISceneTreeView.h"
#include "BsGUIResourceTreeView.h"

namespace BansheeEngine
{
	DbgEditorWidget1* DbgEditorWidget1::Instance = nullptr;

	DbgEditorWidget1::DbgEditorWidget1(const ConstructPrivately& dummy, EditorWidgetContainer& parentContainer)
		:EditorWidget<DbgEditorWidget1>(HString(L"DbgEditorWidget1"), parentContainer)
	{
		GUIScrollArea* scrollArea = GUIScrollArea::create();
		mContent->addElement(scrollArea);

		GUILayout* treeViewLayout = scrollArea->getLayout().addNewElement<GUILayoutY>();

		mSceneTreeView = GUISceneTreeView::create(GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()));
		mResourceTreeView = GUIResourceTreeView::create(GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()));

		treeViewLayout->addElement(GUILabel::create(HString(L"<<<<<<<<<<<<<<SCENE VIEW>>>>>>>>>>>>>>")));
		treeViewLayout->addElement(mSceneTreeView);

		treeViewLayout->addElement(GUILabel::create(HString(L"<<<<<<<<<<<<<<RESOURCE VIEW>>>>>>>>>>>>>>")));
		treeViewLayout->addElement(mResourceTreeView);
	}

	DbgEditorWidget1::~DbgEditorWidget1()
	{

	}

	void DbgEditorWidget1::update()
	{
		mSceneTreeView->update();
		mResourceTreeView->update();
	}

	DbgEditorWidget1* DbgEditorWidget1::instance()
	{
		return Instance;
	}

	DbgEditorWidget1* DbgEditorWidget1::open()
	{
		return static_cast<DbgEditorWidget1*>(EditorWidgetManager::instance().open(getTypeName()));
	}

	void DbgEditorWidget1::close()
	{
		if(Instance != nullptr)
			EditorWidgetManager::instance().close(Instance);
		
		Instance = nullptr; 
	}

	const String& DbgEditorWidget1::getTypeName()
	{
		static String name = "DbgEditorWidget1";
		return name;
	}
}