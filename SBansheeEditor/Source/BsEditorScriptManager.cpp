#include "BsEditorScriptManager.h"
#include "BsScriptEditorWindow.h"
#include "BsScriptObjectManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsScriptAssemblyManager.h"
#include "BsScriptGizmoManager.h"
#include "BsScriptHandleManager.h"
#include "BsScriptHandleSliderManager.h"
#include "BsTime.h"
#include "BsMath.h"

namespace BansheeEngine
{
	const float EditorScriptManager::EDITOR_UPDATE_RATE = 1.0f/60.0f; // Seconds
	bool EditorScriptManager::mDebugRefresh = false;

	EditorScriptManager::EditorScriptManager()
		:mEditorAssembly(nullptr), mProgramEdClass(nullptr), mUpdateMethod(nullptr)
	{
		loadMonoTypes();
		ScriptAssemblyManager::instance().loadAssemblyInfo(BansheeEditorAssemblyName);

		ScriptHandleSliderManager::startUp();
		ScriptGizmoManager::startUp(ScriptAssemblyManager::instance());
		HandleManager::startUp<ScriptHandleManager>(ScriptAssemblyManager::instance());

		mOnDomainLoadConn = ScriptObjectManager::instance().onRefreshDomainLoaded.connect(std::bind(&EditorScriptManager::loadMonoTypes, this));
		mOnAssemblyRefreshDoneConn = ScriptObjectManager::instance().onRefreshComplete.connect(std::bind(&EditorScriptManager::onAssemblyRefreshDone, this));
		triggerOnInitialize();
		
		// Initial update
		mLastUpdateTime = gTime().getTime();
		mUpdateMethod->invoke(nullptr, nullptr);
	}

	EditorScriptManager::~EditorScriptManager()
	{
		mOnDomainLoadConn.disconnect();
		mOnAssemblyRefreshDoneConn.disconnect();

		ScriptHandleSliderManager::shutDown();
		HandleManager::shutDown();
		ScriptGizmoManager::shutDown();
	}

	void EditorScriptManager::update()
	{
		float curTime = gTime().getTime();
		float diff = curTime - mLastUpdateTime;

		if (diff > EDITOR_UPDATE_RATE)
		{
			mUpdateMethod->invoke(nullptr, nullptr);

			INT32 numUpdates = Math::floorToInt(diff / EDITOR_UPDATE_RATE);
			mLastUpdateTime += numUpdates * EDITOR_UPDATE_RATE;
		}

		if (mDebugRefresh)
		{
			ScriptObjectManager::instance().refreshAssemblies();
			mDebugRefresh = false;
		}

		ScriptGizmoManager::instance().update();
	}

	void EditorScriptManager::debug_refreshAssembly()
	{
		mDebugRefresh = true;
	}

	void EditorScriptManager::triggerOnInitialize()
	{
		const String ASSEMBLY_ENTRY_POINT = "Program::OnInitialize";
		mEditorAssembly->invoke(ASSEMBLY_ENTRY_POINT);
	}

	void EditorScriptManager::onAssemblyRefreshDone()
	{
		triggerOnInitialize();
	}

	void EditorScriptManager::loadMonoTypes()
	{
		const String ENGINE_ASSEMBLY_PATH = "..\\..\\Assemblies\\MBansheeEditor.dll";
		const String ENGINE_ASSEMBLY_NAME = BansheeEditorAssemblyName;
		
		mEditorAssembly = &MonoManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME);

		mProgramEdClass = mEditorAssembly->getClass("BansheeEditor", "Program");
		mUpdateMethod = mProgramEdClass->getMethod("EditorUpdate");

		ScriptEditorWindow::clearRegisteredEditorWindow();
		ScriptEditorWindow::registerManagedEditorWindows();

		// DEBUG ONLY
		MonoClass* debugWindowClass = mEditorAssembly->getClass("BansheeEditor", "DebugWindow");
		debugWindowClass->addInternalCall("Internal_RefreshAssembly", &EditorScriptManager::debug_refreshAssembly);
	}
}