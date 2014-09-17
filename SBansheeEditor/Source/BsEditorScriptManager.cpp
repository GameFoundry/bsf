#include "BsEditorScriptManager.h"
#include "BsScriptEditorWindow.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptGizmoManager.h"
#include "BsTime.h"
#include "BsMath.h"

namespace BansheeEngine
{
	const float EditorScriptManager::EDITOR_UPDATE_RATE = 1.0f/60.0f; // Seconds

	EditorScriptManager::EditorScriptManager()
		:mEditorAssembly(nullptr), mProgramEdClass(nullptr), mUpdateMethod(nullptr)
	{
		const String ENGINE_ASSEMBLY_PATH = "..\\..\\Assemblies\\MBansheeEditor.dll";
		const String ENGINE_ASSEMBLY_NAME = BansheeEditorAssemblyName;
		const String ASSEMBLY_ENTRY_POINT = "ProgramEd::Main";

		mEditorAssembly = &MonoManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME);
		ScriptEditorWindow::registerManagedEditorWindows();

		RuntimeScriptObjects::instance().refreshScriptObjects(BansheeEditorAssemblyName);

		ScriptGizmoManager::startUp(RuntimeScriptObjects::instance());

		mProgramEdClass = mEditorAssembly->getClass("BansheeEditor", "ProgramEd");
		mUpdateMethod = &mProgramEdClass->getMethod("EditorUpdate");

		mEditorAssembly->invoke(ASSEMBLY_ENTRY_POINT);
		
		// Initial update
		mLastUpdateTime = gTime().getTime();
		mUpdateMethod->invoke(nullptr, nullptr);
	}

	EditorScriptManager::~EditorScriptManager()
	{
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

		ScriptGizmoManager::instance().update();
	}
}