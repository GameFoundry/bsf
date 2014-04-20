#include "BsScriptEditorWindow.h"
#include "BsScriptMeta.h"
#include "BsScriptGUIPanel.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsEditorWidget.h"
#include "BsEditorWidgetManager.h"
#include "BsEditorWidgetContainer.h"
#include "BsMonoAssembly.h"

using namespace CamelotFramework;
using namespace BansheeEngine;
using namespace std::placeholders;

namespace BansheeEditor
{
	UnorderedMap<String, ScriptEditorWindow::EditorWindowHandle>::type ScriptEditorWindow::OpenScriptEditorWindows;
	BS::MonoMethod* ScriptEditorWindow::onResizedMethod = nullptr;

	ScriptEditorWindow::ScriptEditorWindow(const CM::String& windowName, const CM::String& displayName, EditorWidgetBase* editorWidget)
		:mName(windowName), mEditorWidget(editorWidget)
	{
		mOnWidgetMovedConn = editorWidget->onMoved.connect(std::bind(&ScriptEditorWindow::onWidgetMoved, this, _1, _2));
		mOnWidgetResizedConn = editorWidget->onResized.connect(std::bind(&ScriptEditorWindow::onWidgetResized, this, _1, _2));
		mOnParentChangedConn = editorWidget->onParentChanged.connect(std::bind(&ScriptEditorWindow::onWidgetParentChanged, this, _1));
	}

	ScriptEditorWindow::~ScriptEditorWindow()
	{
		// TODO - This probably need to be called whenever we close a window, and not when script class is destructed
		mOnWidgetMovedConn.disconnect();
		mOnWidgetResizedConn.disconnect();
		mOnParentChangedConn.disconnect();
	}

	void ScriptEditorWindow::initMetaData()
	{
		metaData = ScriptMeta(BansheeEditorAssemblyName, "BansheeEditor", "EditorWindow", &ScriptEditorWindow::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptEditorWindow::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateOrGetInstance", &ScriptEditorWindow::internal_createOrGetInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptEditorWindow::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_InitializeGUIPanel", &ScriptEditorWindow::internal_initializeGUIPanel);
		metaData.scriptClass->addInternalCall("Internal_GetWidth", &ScriptEditorWindow::internal_getWidth);
		metaData.scriptClass->addInternalCall("Internal_GetHeight", &ScriptEditorWindow::internal_getHeight);

		onResizedMethod = &metaData.scriptClass->getMethod("WindowResized", 2);
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

	void ScriptEditorWindow::internal_destroyInstance(ScriptEditorWindow* thisPtr)
	{
#if CM_DEBUG_MODE
		auto iterFind = OpenScriptEditorWindows.find(thisPtr->mName);

		// It is assumed that this method will only be called after "unregisterScriptEditorWindow" is called,
		// since that is the only place keeping a reference to the managed editor window. So if window was
		// not removed from OpenScriptEditorWindows array, then something went wrong earlier.
		assert(iterFind == OpenScriptEditorWindows.end());
#endif

		cm_delete(thisPtr);
	}

	CM::UINT32 ScriptEditorWindow::internal_getWidth(ScriptEditorWindow* thisPtr)
	{
		return thisPtr->mEditorWidget->getWidth();
	}

	CM::UINT32 ScriptEditorWindow::internal_getHeight(ScriptEditorWindow* thisPtr)
	{
		return thisPtr->mEditorWidget->getHeight();
	}

	void ScriptEditorWindow::internal_initializeGUIPanel(ScriptEditorWindow* thisPtr, MonoObject* panel)
	{
		ScriptGUIPanel* scriptGUIPanel = ScriptGUIPanel::toNative(panel);
		thisPtr->mPanels.push_back(scriptGUIPanel);

		scriptGUIPanel->setParentArea(thisPtr->mEditorWidget->getX(), thisPtr->mEditorWidget->getY(), 
			thisPtr->mEditorWidget->getWidth(), thisPtr->mEditorWidget->getHeight());

		scriptGUIPanel->setParentWidget(&thisPtr->mEditorWidget->_getParent()->getParentWidget());
	}

	void ScriptEditorWindow::onWidgetMoved(CM::INT32 x, CM::INT32 y)
	{
		for(auto& panel : mPanels)
		{
			panel->setParentArea(x, y, 
				mEditorWidget->getWidth(), mEditorWidget->getHeight());
		}
	}

	void ScriptEditorWindow::onWidgetResized(CM::UINT32 width, CM::UINT32 height)
	{
		for(auto& panel : mPanels)
		{
			panel->setParentArea(mEditorWidget->getX(), mEditorWidget->getY(), 
				width, height);
		}

		void* params[2] = { &width, &height };
		onResizedMethod->invokeVirtual(mManagedInstance, params);
	}

	void ScriptEditorWindow::onWidgetParentChanged(EditorWidgetContainer* newParent)
	{
		for(auto& panel : mPanels)
		{
			if(newParent != nullptr)
				panel->setParentWidget(&newParent->getParentWidget());
			else
				panel->setParentWidget(nullptr);
		}
	}

	void ScriptEditorWindow::registerManagedEditorWindows()
	{
		BS::MonoAssembly* assembly = MonoManager::instance().getAssembly(BansheeEditorAssemblyName);

		if(assembly != nullptr)
		{
			BS::MonoClass* editorWindowClass = assembly->getClass("BansheeEditor", "EditorWindow");

			const CM::Vector<BS::MonoClass*>::type& allClasses = assembly->getAllClasses();
			for(auto& curClass : allClasses)
			{
				if(curClass->isSubClassOf(editorWindowClass) && curClass != editorWindowClass)
				{
					const String& className = curClass->getFullName();
					EditorWidgetManager::instance().registerWidget(className, 
						std::bind(&ScriptEditorWindow::openEditorWidgetCallback, curClass->getNamespace(), curClass->getTypeName(), std::placeholders::_1));
				}
			}
		}
	}

	EditorWidgetBase* ScriptEditorWindow::openEditorWidgetCallback(CM::String ns, CM::String type, EditorWidgetContainer& parentContainer)
	{
		BS::MonoAssembly* assembly = MonoManager::instance().getAssembly(BansheeEditorAssemblyName);

		if(assembly == nullptr)
			return nullptr;

		BS::MonoClass* editorWindowClass = assembly->getClass(ns, type);

		if(editorWindowClass == nullptr)
			return nullptr;

		MonoObject* editorWindowInstance = editorWindowClass->createInstance(false);
		if(editorWindowInstance == nullptr)
			return nullptr;

		String windowFullTypeName = MonoManager::instance().getFullTypeName(editorWindowInstance);
		String displayName = MonoManager::instance().getTypeName(editorWindowInstance);

		ScriptEditorWidget* editorWidget = cm_new<ScriptEditorWidget>(windowFullTypeName, HString(toWString(displayName)), parentContainer);
		ScriptEditorWindow* nativeInstance = new (cm_alloc<ScriptEditorWindow>()) ScriptEditorWindow(windowFullTypeName, displayName, editorWidget);
		nativeInstance->createInstance(editorWindowInstance);

		metaData.thisPtrField->setValue(editorWindowInstance, &nativeInstance);

		ScriptEditorWindow::registerScriptEditorWindow(nativeInstance);

		mono_runtime_object_init(editorWindowInstance); // Construct it

		return editorWidget;
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

	void ScriptEditorWindow::unregisterScriptEditorWindow(const CM::String& windowTypeName)
	{
		auto findIter = OpenScriptEditorWindows.find(windowTypeName);
		if(findIter != OpenScriptEditorWindows.end())
		{
			EditorWindowHandle& foundHandle = findIter->second;
			mono_gchandle_free(foundHandle.gcHandle);

			OpenScriptEditorWindows.erase(findIter);
		}
	}

	ScriptEditorWidget::ScriptEditorWidget(const String& windowTypeName, const HString& displayName, EditorWidgetContainer& parentContainer)
		:EditorWidgetBase(displayName, windowTypeName, parentContainer), mWindowTypeName(windowTypeName)
	{
		
	}

	ScriptEditorWidget::~ScriptEditorWidget()
	{
		ScriptEditorWindow::unregisterScriptEditorWindow(mWindowTypeName);
	}
}