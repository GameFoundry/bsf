#include "DbgEditorWidget2.h"
#include "BsEditorWidgetManager.h"
#include "BsGUIToggle.h"
#include "BsGUIScrollArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUIIntField.h"
#include "BsGUIFloatField.h"
#include "BsGUITextField.h"
#include "BsGUIVector2Field.h"
#include "BsGUIVector3Field.h"
#include "BsGUIVector4Field.h"
#include "BsGUIToggleField.h"
#include "BsGUIColorField.h"
#include "BsGUIButton.h"
#include "BsGUIComponentFoldout.h"
#include "BsGUISpace.h"
#include "BsHString.h"

#include "BsCoreThread.h"
#include "BsCoreApplication.h"

namespace BansheeEngine
{
	bool bs_dbg_fullscreen = false;

	void toggleFullscreen()
	{
		if (bs_dbg_fullscreen)
		{
			gCoreAccessor().setWindowed(gCoreApplication().getPrimaryWindow(), 1280, 720);
		}
		else
		{
			//gCoreAccessor().setFullscreen(window, *videoMode);
			gCoreAccessor().setFullscreen(gCoreApplication().getPrimaryWindow(), 1920, 1200, 60, 0);
		}

		bs_dbg_fullscreen = !bs_dbg_fullscreen;
	}


	DbgEditorWidget2* DbgEditorWidget2::Instance = nullptr;

	DbgEditorWidget2::DbgEditorWidget2(const ConstructPrivately& dummy, EditorWidgetContainer& parentContainer)
		:EditorWidget<DbgEditorWidget2>(HString(L"DbgEditorWidget2"), parentContainer)
	{
		GUILayout& layout = mContent->getLayout().addLayoutY();

		GUIIntField* intField = GUIIntField::create(GUIContent(HString(L"Int Field")), 100, GUIOptions(GUIOption::fixedWidth(200)));
		GUIFloatField* floatField = GUIFloatField::create(HString(L"Float Field"), GUIOptions(GUIOption::fixedWidth(200)));
		GUITextField* textField = GUITextField::create(false, HString(L"Text Field"), GUIOptions(GUIOption::fixedWidth(200)));
		GUIVector4Field* vec4Field = GUIVector4Field::create(HString(L"Vec4 Field"), GUIOptions(GUIOption::fixedWidth(200)));
		GUIVector3Field* vec3Field = GUIVector3Field::create(HString(L"Vec3 Field"), GUIOptions(GUIOption::fixedWidth(200)));
		GUIVector2Field* vec2Field = GUIVector2Field::create(HString(L"Vec2 Field"), GUIOptions(GUIOption::fixedWidth(200)));
		GUIToggleField* toggleField = GUIToggleField::create(HString(L"Toggle Field"), GUIOptions(GUIOption::fixedWidth(200)));
		GUIColorField* colorField = GUIColorField::create(HString(L"Color Field"), GUIOptions(GUIOption::fixedWidth(200)));
		GUIComponentFoldout* foldout = GUIComponentFoldout::create(HString(L"Test"), GUIOptions(GUIOption::fixedWidth(200)));
		colorField->setValue(Color::Red);

		GUIButton* button = GUIButton::create(HString(L"Testing"), GUIOptions(GUIOption::fixedWidth(100)));
		button->onClick.connect(&toggleFullscreen);

		layout.addElement(intField);
		layout.addElement(floatField);
		layout.addElement(textField);
		layout.addElement(vec4Field);
		layout.addElement(vec3Field);
		layout.addElement(vec2Field);
		layout.addElement(toggleField);
		layout.addElement(colorField);
		layout.addElement(foldout);
		layout.addElement(button);

		layout.addFlexibleSpace();
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