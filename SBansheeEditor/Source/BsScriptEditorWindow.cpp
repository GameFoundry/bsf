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

using namespace std::placeholders;

namespace BansheeEngine
{
	UnorderedMap<String, ScriptEditorWindow::EditorWindowHandle> ScriptEditorWindow::OpenScriptEditorWindows;
	MonoMethod* ScriptEditorWindow::onResizedMethod = nullptr;

	ScriptEditorWindow::ScriptEditorWindow(MonoObject* instance, const String& windowName, const String& displayName, EditorWidgetBase* editorWidget)
		:ScriptObject(instance), mName(windowName), mEditorWidget(editorWidget)
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

	void ScriptEditorWindow::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateOrGetInstance", &ScriptEditorWindow::internal_createOrGetInstance);
		metaData.scriptClass->addInternalCall("Internal_InitializeGUIPanel", &ScriptEditorWindow::internal_initializeGUIPanel);
		metaData.scriptClass->addInternalCall("Internal_GetWidth", &ScriptEditorWindow::internal_getWidth);
		metaData.scriptClass->addInternalCall("Internal_GetHeight", &ScriptEditorWindow::internal_getHeight);
		metaData.scriptClass->addInternalCall("Internal_HasFocus", &ScriptEditorWindow::internal_hasFocus);
		metaData.scriptClass->addInternalCall("Internal_ScreenToWindowPos", &ScriptEditorWindow::internal_screenToWindowPos);
		metaData.scriptClass->addInternalCall("Internal_WindowToScreenPos", &ScriptEditorWindow::internal_windowToScreenPos);

		onResizedMethod = metaData.scriptClass->getMethod("WindowResized", 2);
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

	void ScriptEditorWindow::_onManagedInstanceDeleted()
	{
		unregisterScriptEditorWindow(mName);

		ScriptObject::_onManagedInstanceDeleted();
	}

	bool ScriptEditorWindow::internal_hasFocus(ScriptEditorWindow* thisPtr)
	{
		return thisPtr->getEditorWidget()->hasFocus();
	}

	Vector2I ScriptEditorWindow::internal_screenToWindowPos(ScriptEditorWindow* thisPtr, const Vector2I& screenPos)
	{
		return thisPtr->getEditorWidget()->screenToWidgetPos(screenPos);
	}

	Vector2I ScriptEditorWindow::internal_windowToScreenPos(ScriptEditorWindow* thisPtr, const Vector2I& windowPos)
	{
		return thisPtr->getEditorWidget()->widgetToScreenPos(windowPos);
	}

	UINT32 ScriptEditorWindow::internal_getWidth(ScriptEditorWindow* thisPtr)
	{
		return thisPtr->mEditorWidget->getWidth();
	}

	UINT32 ScriptEditorWindow::internal_getHeight(ScriptEditorWindow* thisPtr)
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

	void ScriptEditorWindow::onWidgetMoved(INT32 x, INT32 y)
	{
		for(auto& panel : mPanels)
		{
			panel->setParentArea(x, y, 
				mEditorWidget->getWidth(), mEditorWidget->getHeight());
		}
	}

	void ScriptEditorWindow::onWidgetResized(UINT32 width, UINT32 height)
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
		MonoAssembly* assembly = MonoManager::instance().getAssembly(BansheeEditorAssemblyName);

		if(assembly != nullptr)
		{
			MonoClass* editorWindowClass = assembly->getClass("BansheeEditor", "EditorWindow");

			const Vector<MonoClass*>& allClasses = assembly->getAllClasses();
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

	EditorWidgetBase* ScriptEditorWindow::openEditorWidgetCallback(String ns, String type, EditorWidgetContainer& parentContainer)
	{
		MonoAssembly* assembly = MonoManager::instance().getAssembly(BansheeEditorAssemblyName);

		if(assembly == nullptr)
			return nullptr;

		MonoClass* editorWindowClass = assembly->getClass(ns, type);

		if(editorWindowClass == nullptr)
			return nullptr;

		MonoObject* editorWindowInstance = editorWindowClass->createInstance(false);
		if(editorWindowInstance == nullptr)
			return nullptr;

		String windowFullTypeName = MonoManager::instance().getFullTypeName(editorWindowInstance);
		String displayName = MonoManager::instance().getTypeName(editorWindowInstance);

		ScriptEditorWidget* editorWidget = bs_new<ScriptEditorWidget>(windowFullTypeName, HString(toWString(displayName)), parentContainer);
		ScriptEditorWindow* nativeInstance = new (bs_alloc<ScriptEditorWindow>()) ScriptEditorWindow(editorWindowInstance, windowFullTypeName, displayName, editorWidget);

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

	void ScriptEditorWindow::unregisterScriptEditorWindow(const String& windowTypeName)
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