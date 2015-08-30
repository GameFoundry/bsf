#include "BsScriptDropDownWindow.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsMonoAssembly.h"
#include "BsRenderWindow.h"
#include "BsScriptGUILayout.h"
#include "BsScriptEditorWindow.h"
#include "BsEditorWidget.h"
#include "BsEditorWindow.h"
#include "BsEditorWidgetContainer.h"
#include "BsCGUIWidget.h"
#include "BsDropDownWindowManager.h"
#include <BsScriptObjectManager.h>

using namespace std::placeholders;

namespace BansheeEngine
{
	MonoField* ScriptDropDownWindow::guiPanelField = nullptr;

	ScriptDropDownWindow::ScriptDropDownWindow(ManagedDropDownWindow* window)
		:ScriptObject(window->getManagedInstance()), mDropDownWindow(window)
	{
		mOnAssemblyRefreshStartedConn = ScriptObjectManager::instance().onRefreshStarted.connect(std::bind(&ScriptDropDownWindow::onAssemblyRefreshStarted, this));
	}

	ScriptDropDownWindow::~ScriptDropDownWindow()
	{
		mOnAssemblyRefreshStartedConn.disconnect();

		// Window must have been marked as deleted already
		assert(mDropDownWindow == nullptr);
	}

	void ScriptDropDownWindow::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptDropDownWindow::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_Close", &ScriptDropDownWindow::internal_Close);
		metaData.scriptClass->addInternalCall("Internal_SetWidth", &ScriptDropDownWindow::internal_SetWidth);
		metaData.scriptClass->addInternalCall("Internal_SetHeight", &ScriptDropDownWindow::internal_SetHeight);
		metaData.scriptClass->addInternalCall("Internal_ScreenToWindowPos", &ScriptDropDownWindow::internal_ScreenToWindowPos);
		metaData.scriptClass->addInternalCall("Internal_WindowToScreenPos", &ScriptDropDownWindow::internal_WindowToScreenPos);

		guiPanelField = metaData.scriptClass->getField("GUI");
	}

	void ScriptDropDownWindow::internal_CreateInstance(MonoObject* instance, ScriptEditorWindow* parentWindow, 
		Vector2I position, int width, int height)
	{
		ManagedDropDownWindow* dropDownWindow = nullptr;
		if (parentWindow != nullptr && !parentWindow->isDestroyed())
		{
			EditorWidgetBase* editorWidget = parentWindow->getEditorWidget();
			EditorWidgetContainer* parentContainer = editorWidget->_getParent();
			if (parentContainer != nullptr)
			{
				RenderWindowPtr parentRenderWindow = parentContainer->getParentWindow()->getRenderWindow();
				CameraPtr parentCamera = parentContainer->getParentWidget().getCamera();

				position.x += editorWidget->getX();
				position.y += editorWidget->getY();

				dropDownWindow = DropDownWindowManager::instance().open<ManagedDropDownWindow>(
					parentRenderWindow, parentCamera, position, instance, width, height);
			}
		}

		ScriptDropDownWindow* nativeInstance = new (bs_alloc<ScriptDropDownWindow>()) ScriptDropDownWindow(dropDownWindow);

		if (dropDownWindow != nullptr)
		{
			dropDownWindow->initialize(nativeInstance);
			dropDownWindow->triggerOnInitialize();
		}
	}

	void ScriptDropDownWindow::internal_Close(ScriptDropDownWindow* thisPtr)
	{
		if (thisPtr->mDropDownWindow != nullptr)
			DropDownWindowManager::instance().close();
	}

	void ScriptDropDownWindow::onAssemblyRefreshStarted()
	{
		if (mDropDownWindow != nullptr)
			DropDownWindowManager::instance().close();
	}

	void ScriptDropDownWindow::notifyWindowClosed()
	{
		mDropDownWindow = nullptr;
	}

	void ScriptDropDownWindow::internal_SetWidth(ScriptDropDownWindow* thisPtr, UINT32 value)
	{
		if (thisPtr->mDropDownWindow != nullptr)
			thisPtr->mDropDownWindow->setSize(value, thisPtr->mDropDownWindow->getHeight());
	}

	void ScriptDropDownWindow::internal_SetHeight(ScriptDropDownWindow* thisPtr, UINT32 value)
	{
		if (thisPtr->mDropDownWindow != nullptr)
			thisPtr->mDropDownWindow->setSize(thisPtr->mDropDownWindow->getWidth(), value);
	}

	void ScriptDropDownWindow::internal_ScreenToWindowPos(ScriptDropDownWindow* thisPtr, Vector2I screenPos, Vector2I* windowPos)
	{
		if (thisPtr->mDropDownWindow != nullptr)
			*windowPos = thisPtr->mDropDownWindow->screenToWindowPos(screenPos);
		else
			*windowPos = screenPos;
	}

	void ScriptDropDownWindow::internal_WindowToScreenPos(ScriptDropDownWindow* thisPtr, Vector2I windowPos, Vector2I* screenPos)
	{
		if (thisPtr->mDropDownWindow != nullptr)
			*screenPos = thisPtr->mDropDownWindow->windowToScreenPos(windowPos);
		else
			*screenPos = windowPos;
	}

	ManagedDropDownWindow::ManagedDropDownWindow(const RenderWindowPtr& parent, const CameraPtr& camera,
		const Vector2I& position, MonoObject* managedInstance, UINT32 width, UINT32 height)
		:DropDownWindow(parent, camera, position, width, height), mUpdateThunk(nullptr), mManagedInstance(managedInstance),
		mOnInitializeThunk(nullptr), mOnDestroyThunk(nullptr), mGCHandle(0), mScriptParent(nullptr), mContentsPanel(nullptr)
	{
		mGCHandle = mono_gchandle_new(mManagedInstance, false);

		MonoObject* guiPanel = ScriptGUIPanel::createFromExisting(mContents);
		mContentsPanel = ScriptGUILayout::toNative(guiPanel);
		ScriptDropDownWindow::guiPanelField->setValue(mManagedInstance, guiPanel);

		::MonoClass* rawMonoClass = mono_object_get_class(mManagedInstance);
		MonoClass* monoClass = MonoManager::instance().findClass(rawMonoClass);

		mNamespace = monoClass->getNamespace();
		mTypename = monoClass->getTypeName();

		reloadMonoTypes(monoClass);
	}

	ManagedDropDownWindow::~ManagedDropDownWindow()
	{
		mContentsPanel->destroyChildren();
		mContentsPanel->markAsDestroyed();
		mContentsPanel = nullptr;

		triggerOnDestroy();
		mScriptParent->notifyWindowClosed();

		mono_gchandle_free(mGCHandle);
		mGCHandle = 0;
	}

	void ManagedDropDownWindow::initialize(ScriptDropDownWindow* parent)
	{
		mScriptParent = parent;
	}

	void ManagedDropDownWindow::triggerOnInitialize()
	{
		if (mOnInitializeThunk != nullptr && mManagedInstance != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mOnInitializeThunk, mManagedInstance);
		}
	}

	void ManagedDropDownWindow::triggerOnDestroy()
	{
		if (mOnDestroyThunk != nullptr && mManagedInstance != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mOnDestroyThunk, mManagedInstance);
		}
	}

	void ManagedDropDownWindow::update()
	{
		if (mUpdateThunk != nullptr && mManagedInstance != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mUpdateThunk, mManagedInstance);
		}
	}

	void ManagedDropDownWindow::reloadMonoTypes(MonoClass* windowClass)
	{
		MonoMethod* updateMethod = windowClass->getMethod("OnEditorUpdate", 0);

		if (updateMethod != nullptr)
			mUpdateThunk = (UpdateThunkDef)updateMethod->getThunk();

		MonoMethod* onInitializeMethod = windowClass->getMethod("OnInitialize", 0);

		if (onInitializeMethod != nullptr)
			mOnInitializeThunk = (OnInitializeThunkDef)onInitializeMethod->getThunk();

		MonoMethod* onDestroyMethod = windowClass->getMethod("OnDestroy", 0);

		if (onDestroyMethod != nullptr)
			mOnDestroyThunk = (OnDestroyThunkDef)onDestroyMethod->getThunk();
	}
}