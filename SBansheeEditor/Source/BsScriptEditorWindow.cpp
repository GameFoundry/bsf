#include "BsScriptEditorWindow.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsEditorWidget.h"
#include "BsEditorWidgetManager.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	UnorderedMap<String, ScriptEditorWindow*>::type ScriptEditorWindow::OpenScriptEditorWindows;

	ScriptEditorWindow::ScriptEditorWindow(const CM::String& windowName)
		:mWidget(nullptr)
	{
		mWidget = EditorWidgetManager::instance().open(windowName);
	}

	ScriptEditorWindow::~ScriptEditorWindow()
	{
		if(mWidget != nullptr)
			mWidget->close();
	}

	void ScriptEditorWindow::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEditor", "BansheeEditor", "EditorWindow", &ScriptEditorWindow::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptEditorWindow::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateOrGetInstance", &ScriptEditorWindow::internal_createOrGetInstance);
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptEditorWindow::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptEditorWindow::internal_destroyInstance);
	}

	MonoObject* ScriptEditorWindow::internal_createOrGetInstance(MonoString* ns, MonoString* typeName)
	{
		String strTypeName = toString(MonoUtil::monoToWString(typeName));
		String strNamespace = toString(MonoUtil::monoToWString(ns));
		String fullName = strNamespace + "::" + strTypeName;

		auto findIter = OpenScriptEditorWindows.find(fullName);
		if(findIter != OpenScriptEditorWindows.end())
			return findIter->second->mManagedInstance;

		MonoObject* instance = MonoManager::instance().createInstance(strNamespace, strTypeName);
		if(instance == nullptr)
			CM_EXCEPT(InvalidParametersException, "Provided name is not a valid type name \"" + fullName + "\"");

		return instance;
	}

	void ScriptEditorWindow::internal_createInstance(MonoObject* instance)
	{
		String windowTypeName = MonoManager::instance().getTypeName(instance);

		ScriptEditorWindow* nativeInstance = new (cm_alloc<ScriptEditorWindow>()) ScriptEditorWindow(windowTypeName);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptEditorWindow::internal_destroyInstance(ScriptEditorWindow* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}