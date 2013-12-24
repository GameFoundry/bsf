#include "BsEngineAssembly.h"
#include "BsScriptManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const CM::String EngineAssembly::ENGINE_ASSEMBLY_PATH = "..\\..\\Assemblies\\MBansheeEngine.dll";
	const CM::String EngineAssembly::ENGINE_ASSEMBLY_NAME = "MBansheeEngine";
	const CM::String EngineAssembly::ASSEMBLY_ENTRY_POINT = "Program::Main";

	EngineAssembly::EngineAssembly()
	{
		mAssembly = &ScriptManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME, ASSEMBLY_ENTRY_POINT);
	}

	EngineAssembly::~EngineAssembly()
	{
		ScriptManager::instance().unloadAssembly(*mAssembly);
	}

	ScriptAssembly& gEngineAssembly()
	{
		return EngineAssembly::instance().getAssembly();
	}
}