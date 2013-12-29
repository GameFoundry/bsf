#include "BsScriptEditorPrerequisites.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	extern "C" BS_SCR_BED_EXPORT const String& getPluginName()
	{
		static String pluginName = "SBansheeEditor";
		return pluginName;
	}

	extern "C" BS_SCR_BED_EXPORT void* loadPlugin()
	{
		const CM::String ENGINE_ASSEMBLY_PATH = "..\\..\\Assemblies\\MBansheeEditor.dll";
		const CM::String ENGINE_ASSEMBLY_NAME = "MBansheeEditor";
		const CM::String ASSEMBLY_ENTRY_POINT = "Program::Main";

		MonoManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME, ASSEMBLY_ENTRY_POINT);

		return nullptr;
	}
}