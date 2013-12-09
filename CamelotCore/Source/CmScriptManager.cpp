#include "CmScriptManager.h"
#include "CmException.h"
#include "CmScriptAssembly.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

namespace CamelotFramework
{
	const String ScriptManager::DOMAIN_NAME = "BansheeEngine";
	const String ScriptManager::MONO_LIB_DIR = "..\\..\\Mono\\lib";
	const String ScriptManager::MONO_ETC_DIR = "..\\..\\Mono\\etc";

	ScriptManager::ScriptManager()
	{
		mono_set_dirs(MONO_LIB_DIR.c_str(), MONO_ETC_DIR.c_str()); 
		mono_config_parse(nullptr);

		mDomain = mono_jit_init (DOMAIN_NAME.c_str());
		if(mDomain == nullptr)
		{
			CM_EXCEPT(InternalErrorException, "Cannot initialize Mono runtime.");
		}
	}

	ScriptManager::~ScriptManager()
	{
		for(auto& entry : mAssemblies)
		{
			cm_delete(entry.second);
		}

		mAssemblies.clear();

		if(mDomain != nullptr)
			mono_jit_cleanup(mDomain);
	}

	ScriptAssembly& ScriptManager::loadAssembly(const String& name)
	{
		auto iterFind = mAssemblies.find(name);
		if(iterFind != mAssemblies.end())
		{
			return *iterFind->second;
		}

		MonoAssembly* monoAssembly = mono_domain_assembly_open (mDomain, name.c_str());
		if(monoAssembly == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot load Mono assembly: " + name);
		}

		ScriptAssembly* assembly = new (cm_alloc<ScriptAssembly>()) ScriptAssembly(monoAssembly);
		mAssemblies[name] = assembly;

		return *assembly;
	}
}