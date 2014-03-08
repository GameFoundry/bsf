#include "BsScriptEditorWindow.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsEditorWidget.h"
#include "BsEditorWidgetManager.h"
#include "BsMonoAssembly.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	UnorderedMap<String, ScriptEditorWindow::EditorWindowHandle>::type ScriptEditorWindow::OpenScriptEditorWindows;

	ScriptEditorWindow::ScriptEditorWindow(const CM::String& windowName, const CM::String& displayName)
		:mName(windowName)
	{
		// This will be cleaned up by EditorWidgetManager since technically this should be done in the "create"
		// callback but we're doing it here for convenience.
		mEditorWidget = cm_new<ScriptEditorWidget>(HString(toWString(displayName)), this); 
	}

	ScriptEditorWindow::~ScriptEditorWindow()
	{

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
		String fullName = strNamespace + "." + strTypeName;

		auto findIter = OpenScriptEditorWindows.find(fullName);
		if(findIter != OpenScriptEditorWindows.end())
			return findIter->second.nativeObj->mManagedInstance;

		EditorWidgetManager::instance().open(fullName); // This will trigger openEditorWidgetCallback and update OpenScriptEditorWindows

		auto findIter2 = OpenScriptEditorWindows.find(fullName);
		if(findIter2 != OpenScriptEditorWindows.end())
			return findIter2->second.nativeObj->mManagedInstance;

		return nullptr;
	}

	void ScriptEditorWindow::internal_createInstance(MonoObject* instance)
	{
		String windowFullTypeName = MonoManager::instance().getFullTypeName(instance);
		String displayName = MonoManager::instance().getTypeName(instance);

		ScriptEditorWindow* nativeInstance = new (cm_alloc<ScriptEditorWindow>()) ScriptEditorWindow(windowFullTypeName, displayName);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptEditorWindow::internal_destroyInstance(ScriptEditorWindow* nativeInstance)
	{
#if CM_DEBUG_MODE
		auto iterFind = OpenScriptEditorWindows.find(nativeInstance->mName);

		// It is assumed that this method will only be called after "unregisterScriptEditorWindow" is called,
		// since that is the only place keeping a reference to the managed editor window. So if window was
		// not removed from OpenScriptEditorWindows array, then something went wrong earlier.
		assert(iterFind == OpenScriptEditorWindows.end());
#endif

		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	void ScriptEditorWindow::registerManagedEditorWindows()
	{
		BS::MonoAssembly* assembly = MonoManager::instance().getAssembly("MBansheeEditor");

		if(assembly != nullptr)
		{
			BS::MonoClass* editorWindowClass = assembly->getClass("BansheeEditor", "EditorWindow");

			CM::Vector<BS::MonoClass*>::type allClasses = assembly->getAllClasses();
			for(auto& curClass : allClasses)
			{
				if(curClass->isSubClassOf(editorWindowClass) && curClass != editorWindowClass)
				{
					const String& className = curClass->getFullName();
					EditorWidgetManager::instance().registerWidget(className, 
						std::bind(&ScriptEditorWindow::openEditorWidgetCallback, curClass->getNamespace(), curClass->getTypeName()));

					// DEBUG ONLY
					EditorWidgetManager::instance().open(className);
				}
			}
		}
	}

	EditorWidgetBase* ScriptEditorWindow::openEditorWidgetCallback(CM::String ns, CM::String type)
	{
		BS::MonoAssembly* assembly = MonoManager::instance().getAssembly("MBansheeEditor");

		if(assembly == nullptr)
			return nullptr;

		BS::MonoClass* editorWindowClass = assembly->getClass(ns, type);

		if(editorWindowClass == nullptr)
			return nullptr;

		MonoObject* editorWindowInstance = editorWindowClass->createInstance();
		if(editorWindowInstance == nullptr)
			return nullptr;

		ScriptEditorWindow* scriptEditorWindow = ScriptEditorWindow::toNative(editorWindowInstance);
		return scriptEditorWindow->getEditorWidget();
	}

	void ScriptEditorWindow::registerScriptEditorWindow(ScriptEditorWindow* editorWindow)
	{
		if(editorWindow == nullptr)
			return;

		auto findIter = OpenScriptEditorWindows.find(editorWindow->mName);
		if(findIter == OpenScriptEditorWindows.end())
		{
			EditorWindowHandle newHandle;
			newHandle.nativeObj = editorWindow;
			newHandle.gcHandle = mono_gchandle_new(editorWindow->mManagedInstance, false);

			OpenScriptEditorWindows[editorWindow->mName] = newHandle;
		}
	}

	void ScriptEditorWindow::unregisterScriptEditorWindow(ScriptEditorWindow* editorWindow)
	{
		if(editorWindow == nullptr)
			return;

		auto findIter = OpenScriptEditorWindows.find(editorWindow->mName);
		if(findIter != OpenScriptEditorWindows.end())
		{
			EditorWindowHandle& foundHandle = findIter->second;
			mono_gchandle_free(foundHandle.gcHandle);

			OpenScriptEditorWindows.erase(findIter);
		}
	}

	ScriptEditorWidget::ScriptEditorWidget(const CM::HString& displayName, ScriptEditorWindow* window)
		:EditorWidgetBase(displayName, window->mName), mWindow(window)
	{
		ScriptEditorWindow::registerScriptEditorWindow(mWindow);

		// TODO - This is clumsy. Try to create a window in one step
		window->metaData.scriptClass->invokeMethod("Initialize", window->mManagedInstance);
	}

	ScriptEditorWidget::~ScriptEditorWidget()
	{
		ScriptEditorWindow::unregisterScriptEditorWindow(mWindow);
	}
}