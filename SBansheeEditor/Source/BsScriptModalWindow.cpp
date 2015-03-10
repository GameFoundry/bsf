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
#include "BsScriptHString.h"
#include "BsRenderWindow.h"

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
		metaData.scriptClass->addInternalCall("Internal_SetWidth", &ScriptModalWindow::internal_setWidth);
		metaData.scriptClass->addInternalCall("Internal_SetHeight", &ScriptModalWindow::internal_setHeight);
		metaData.scriptClass->addInternalCall("Internal_SetTitle", &ScriptModalWindow::internal_setTitle);
		metaData.scriptClass->addInternalCall("Internal_ScreenToWindowPos", &ScriptModalWindow::internal_screenToWindowPos);
		metaData.scriptClass->addInternalCall("Internal_WindowToScreenPos", &ScriptModalWindow::internal_windowToScreenPos);

		onInitializedInternalMethod = metaData.scriptClass->getMethod("OnInitializeInternal", 0);
		onDestroyInternalMethod = metaData.scriptClass->getMethod("OnDestroyInternal", 0);
	}

	void ScriptModalWindow::internal_createInstance(MonoObject* instance, bool allowCloseButton)
	{
		ManagedModalWindow* modalWindow = bs_new<ManagedModalWindow>(allowCloseButton, instance);
		ScriptModalWindow* nativeInstance = new (bs_alloc<ScriptModalWindow>()) ScriptModalWindow(modalWindow);
		modalWindow->initialize(nativeInstance);
		modalWindow->triggerOnInitialize();
	}

	void ScriptModalWindow::internal_close(ScriptModalWindow* thisPtr)
	{
		thisPtr->closeWindow();
	}

	void ScriptModalWindow::internal_setTitle(ScriptModalWindow* thisPtr, MonoObject* title)
	{
		HString titleStr = HString::dummy();
		if (title != nullptr)
		{
			ScriptHString* textScript = ScriptHString::toNative(title);
			titleStr = textScript->getInternalValue();
		}

		thisPtr->mModalWindow->setTitle(titleStr);
	}

	void ScriptModalWindow::closeWindow()
	{
		if (mModalWindow == nullptr)
			return;

		mModalWindow->triggerOnDestroy();
		mModalWindow->releaseManagedInstance();
		mModalWindow->close();
		mModalWindow = nullptr;
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

	void ScriptModalWindow::internal_setWidth(ScriptModalWindow* thisPtr, UINT32 value)
	{
		if (thisPtr->mModalWindow != nullptr)
			thisPtr->mModalWindow->setSize(value, thisPtr->mModalWindow->getHeight());
	}

	void ScriptModalWindow::internal_setHeight(ScriptModalWindow* thisPtr, UINT32 value)
	{
		if (thisPtr->mModalWindow != nullptr)
			thisPtr->mModalWindow->setSize(thisPtr->mModalWindow->getWidth(), value);
	}

	void ScriptModalWindow::internal_initializeGUIPanel(ScriptModalWindow* thisPtr, MonoObject* panel)
	{
		ScriptGUIPanel* scriptGUIPanel = ScriptGUIPanel::toNative(panel);
		thisPtr->mGUIPanel = scriptGUIPanel;

		Rect2I contentArea = thisPtr->mModalWindow->getContentArea();
		scriptGUIPanel->setParentArea(contentArea.x, contentArea.y, contentArea.width, contentArea.height);
		scriptGUIPanel->setParentWidget(thisPtr->mModalWindow->getGUIWidget().get());
	}

	void ScriptModalWindow::internal_screenToWindowPos(ScriptModalWindow* thisPtr, Vector2I screenPos, Vector2I* windowPos)
	{
		RenderWindowPtr parentRenderWindow = thisPtr->mModalWindow->getRenderWindow();
		*windowPos = parentRenderWindow->screenToWindowPos(screenPos);
	}

	void ScriptModalWindow::internal_windowToScreenPos(ScriptModalWindow* thisPtr, Vector2I windowPos, Vector2I* screenPos)
	{
		RenderWindowPtr parentRenderWindow = thisPtr->mModalWindow->getRenderWindow();
		*screenPos = parentRenderWindow->windowToScreenPos(windowPos);
	}

	ManagedModalWindow::ManagedModalWindow(bool allowCloseButton, MonoObject* managedInstance)
		:ModalWindow(HString::dummy(), allowCloseButton), mUpdateThunk(nullptr), mManagedInstance(managedInstance),
		mOnInitializeThunk(nullptr), mOnDestroyThunk(nullptr), mOnWindowResizedMethod(nullptr), mGCHandle(0),
		mScriptParent(nullptr)
	{
		mGCHandle = mono_gchandle_new(mManagedInstance, false);

		::MonoClass* rawMonoClass = mono_object_get_class(mManagedInstance);
		MonoClass* monoClass = MonoManager::instance().findClass(rawMonoClass);

		mNamespace = monoClass->getNamespace();
		mTypename = monoClass->getTypeName();

		reloadMonoTypes(monoClass);
	}

	ManagedModalWindow::~ManagedModalWindow()
	{
		assert(mGCHandle == 0); // We expect "close" to be called either from C++ or C# before destruction
	}

	bool ManagedModalWindow::createManagedInstance()
	{
		MonoAssembly* assembly = MonoManager::instance().getAssembly(EDITOR_ASSEMBLY);

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

	void ManagedModalWindow::initialize(ScriptModalWindow* parent)
	{
		mScriptParent = parent;
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

		Rect2I contentArea = getContentArea();
		mScriptParent->mGUIPanel->setParentArea(contentArea.x, contentArea.y, contentArea.width, contentArea.height);

		if (mOnWindowResizedMethod != nullptr && mManagedInstance != nullptr)
		{
			void* params[] = { &width, &height };
			mOnWindowResizedMethod->invokeVirtual(mManagedInstance, params);
		}

		ModalWindow::resized();
	}

	void ManagedModalWindow::close()
	{
		mScriptParent->closeWindow();
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

		MonoClass* modalWindowClass = windowClass->getBaseClass();
		mOnWindowResizedMethod = modalWindowClass->getMethod("OnWindowResized", 2);
	}
}