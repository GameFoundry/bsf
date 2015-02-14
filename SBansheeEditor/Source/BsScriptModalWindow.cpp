#include "BsScriptModalWindow.h"
#include "BsScriptMeta.h"
#include "BsScriptGUIPanel.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsMonoAssembly.h"
#include "BsScriptObjectManager.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	MonoMethod* ScriptModalWindow::onInitializedInternalMethod = nullptr;
	MonoMethod* ScriptModalWindow::onDestroyInternalMethod = nullptr;

	ScriptModalWindow::ScriptModalWindow(ManagedModalWindow* window)
		:ScriptObject(window->getManagedInstance()), mModalWindow(window), mRefreshInProgress(false)
	{
		mOnAssemblyRefreshStartedConn = ScriptObjectManager::instance().onRefreshStarted.connect(std::bind(&ScriptModalWindow::onAssemblyRefreshStarted, this));
	}

	ScriptModalWindow::~ScriptModalWindow()
	{
		mOnAssemblyRefreshStartedConn.disconnect();
	}

	void ScriptModalWindow::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptModalWindow::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_Close", &ScriptModalWindow::internal_close);
		metaData.scriptClass->addInternalCall("Internal_InitializeGUIPanel", &ScriptModalWindow::internal_initializeGUIPanel);
		metaData.scriptClass->addInternalCall("Internal_GetWidth", &ScriptModalWindow::internal_getWidth);
		metaData.scriptClass->addInternalCall("Internal_GetHeight", &ScriptModalWindow::internal_getHeight);

		onInitializedInternalMethod = metaData.scriptClass->getMethod("OnInitializeInternal", 0);
		onDestroyInternalMethod = metaData.scriptClass->getMethod("OnDestroyInternal", 0);
	}

	MonoObject* ScriptModalWindow::internal_createInstance(MonoString* ns, MonoString* typeName)
	{
		String strTypeName = toString(MonoUtil::monoToWString(typeName));
		String strNamespace = toString(MonoUtil::monoToWString(ns));
		String fullName = strNamespace + "." + strTypeName;

		ManagedModalWindow* modalWindow = bs_new<ManagedModalWindow>(strNamespace, strTypeName);
		ScriptModalWindow* nativeInstance = new (bs_alloc<ScriptModalWindow>()) ScriptModalWindow(modalWindow);

		mono_runtime_object_init(modalWindow->getManagedInstance()); // Construct it
		modalWindow->triggerOnInitialize();

		return modalWindow->getManagedInstance();
	}

	void ScriptModalWindow::internal_close(ScriptModalWindow* thisPtr)
	{
		if (thisPtr->mModalWindow == nullptr)
			return;

		thisPtr->mModalWindow->triggerOnDestroy();
		thisPtr->mModalWindow->releaseManagedInstance();
		thisPtr->mModalWindow->close();
		thisPtr->mModalWindow = nullptr;
	}

	void ScriptModalWindow::_onManagedInstanceDeleted()
	{
		if (!mRefreshInProgress)
		{
			// Note: This should only ever get triggered after "internal_close" is called

			ScriptObject::_onManagedInstanceDeleted();
		}
		else
			mManagedInstance = nullptr;
	}

	ScriptObjectBackup ScriptModalWindow::beginRefresh()
	{
		mRefreshInProgress = true;

		if (mModalWindow != nullptr)
			mModalWindow->releaseManagedInstance();

		return PersistentScriptObjectBase::beginRefresh();
	}

	void ScriptModalWindow::endRefresh(const ScriptObjectBackup& backupData)
	{
		mRefreshInProgress = false;

		if (mModalWindow != nullptr)
			mManagedInstance = mModalWindow->getManagedInstance();
		else
			mManagedInstance = nullptr;

		if (mManagedInstance != nullptr)
		{
			mModalWindow->triggerOnInitialize();
		}
		else
		{
			// We couldn't restore managed instance because window class cannot be found
			_onManagedInstanceDeleted();
		}

		PersistentScriptObjectBase::endRefresh(backupData);
	}

	void ScriptModalWindow::onAssemblyRefreshStarted()
	{
		if (mModalWindow != nullptr)
			mModalWindow->triggerOnDestroy();
	}

	MonoObject* ScriptModalWindow::_createManagedInstance(bool construct)
	{
		if (mModalWindow != nullptr)
		{
			mModalWindow->createManagedInstance();

			return mModalWindow->getManagedInstance();
		}
		else
			return nullptr;
	}

	UINT32 ScriptModalWindow::internal_getWidth(ScriptModalWindow* thisPtr)
	{
		if (thisPtr->mModalWindow != nullptr)
			return thisPtr->mModalWindow->getWidth();

		return 0;
	}

	UINT32 ScriptModalWindow::internal_getHeight(ScriptModalWindow* thisPtr)
	{
		if (thisPtr->mModalWindow != nullptr)
			return thisPtr->mModalWindow->getHeight();

		return 0;
	}

	void ScriptModalWindow::internal_initializeGUIPanel(ScriptModalWindow* thisPtr, MonoObject* panel)
	{
		ScriptGUIPanel* scriptGUIPanel = ScriptGUIPanel::toNative(panel);
		thisPtr->mGUIPanel = scriptGUIPanel;

		// TODO - Handle TitleBar when I add it
		scriptGUIPanel->setParentArea(0, 0,
			thisPtr->mModalWindow->getWidth(), thisPtr->mModalWindow->getHeight());

		scriptGUIPanel->setParentWidget(thisPtr->mModalWindow->getGUIWidget().get());
	}

	ManagedModalWindow::ManagedModalWindow(const String& ns, const String& type)
		:ModalWindow(), mNamespace(ns), mTypename(type), mUpdateThunk(nullptr), mManagedInstance(nullptr), 
		mOnInitializeThunk(nullptr), mOnDestroyThunk(nullptr), mOnWindowResizedMethod(nullptr), mGCHandle(0)
	{
		createManagedInstance();
	}

	ManagedModalWindow::~ManagedModalWindow()
	{

	}

	bool ManagedModalWindow::createManagedInstance()
	{
		MonoAssembly* assembly = MonoManager::instance().getAssembly(BansheeEditorAssemblyName);

		if (assembly != nullptr)
		{
			MonoClass* editorWindowClass = assembly->getClass(mNamespace, mTypename);

			if (editorWindowClass != nullptr)
			{
				mManagedInstance = editorWindowClass->createInstance();
				mGCHandle = mono_gchandle_new(mManagedInstance, false);

				reloadMonoTypes(editorWindowClass);
				return true;
			}
		}

		return false;
	}

	void ManagedModalWindow::releaseManagedInstance()
	{
		mono_gchandle_free(mGCHandle);
		mGCHandle = 0;
	}

	void ManagedModalWindow::triggerOnInitialize()
	{
		if (mManagedInstance != nullptr)
			ScriptModalWindow::onInitializedInternalMethod->invoke(mManagedInstance, nullptr);

		if (mOnInitializeThunk != nullptr && mManagedInstance != nullptr)
		{
			MonoException* exception = nullptr;

			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			mOnInitializeThunk(mManagedInstance, &exception);

			MonoUtil::throwIfException(exception);
		}
	}

	void ManagedModalWindow::triggerOnDestroy()
	{
		if (mManagedInstance != nullptr)
			ScriptModalWindow::onDestroyInternalMethod->invoke(mManagedInstance, nullptr);

		if (mOnDestroyThunk != nullptr && mManagedInstance != nullptr)
		{
			MonoException* exception = nullptr;

			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			mOnDestroyThunk(mManagedInstance, &exception);

			MonoUtil::throwIfException(exception);
		}
	}

	void ManagedModalWindow::update()
	{
		if (mUpdateThunk != nullptr && mManagedInstance != nullptr)
		{
			MonoException* exception = nullptr;

			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			mUpdateThunk(mManagedInstance, &exception);

			MonoUtil::throwIfException(exception);
		}
	}

	void ManagedModalWindow::resized()
	{
		UINT32 width = getWidth();
		UINT32 height = getHeight();

		// TODO - Handle TitleBar when I add it
		mScriptParent->mGUIPanel->setParentArea(0, 0, width, height);

		if (mOnWindowResizedMethod != nullptr && mManagedInstance != nullptr)
		{
			void* params[] = { &width, &height };
			mOnWindowResizedMethod->invokeVirtual(mManagedInstance, params);
		}
	}

	void ManagedModalWindow::reloadMonoTypes(MonoClass* windowClass)
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

		mOnWindowResizedMethod = windowClass->getMethod("OnWindowResized", 2);
	}
}