#include "BsScriptEditorPrerequisites.h"
#include "BsScriptEditorWindow.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"

namespace BansheeEngine
{
	extern "C" BS_SCR_BED_EXPORT const String& getPluginName()
	{
		static String pluginName = "SBansheeEditor";
		return pluginName;
	}

	extern "C" BS_SCR_BED_EXPORT void* loadPlugin()
	{
		const String ENGINE_ASSEMBLY_PATH = "..\\..\\Assemblies\\MBansheeEditor.dll";
		const String ENGINE_ASSEMBLY_NAME = BansheeEditorAssemblyName;
		const String ASSEMBLY_ENTRY_POINT = "ProgramEd::Main";

		MonoAssembly& assembly = MonoManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME);
		assembly.invoke(ASSEMBLY_ENTRY_POINT);

		ScriptEditorWindow::registerManagedEditorWindows();

		return nullptr;
	}
}