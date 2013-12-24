#include "BsScriptManager.h"
#include "CmException.h"
#include "BsScriptMeta.h"
#include "BsScriptAssembly.h"
#include "BsScriptClass.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String ScriptManager::MONO_LIB_DIR = "..\\..\\Mono\\lib";
	const String ScriptManager::MONO_ETC_DIR = "..\\..\\Mono\\etc";

	ScriptManager::ScriptManager()
	{
		mono_set_dirs(MONO_LIB_DIR.c_str(), MONO_ETC_DIR.c_str()); 
		mono_config_parse(nullptr);
	}

	ScriptManager::~ScriptManager()
	{
		for(auto& entry : mAssemblies)
		{
			unloadAssembly(*entry.second);
			cm_delete(entry.second);
		}

		mAssemblies.clear();
	}

	ScriptAssembly& ScriptManager::loadAssembly(const String& path, const String& name, const String& entryPoint)
	{
		ScriptAssembly* assembly = nullptr;

		auto iterFind = mAssemblies.find(name);
		if(iterFind != mAssemblies.end())
		{
			assembly = iterFind->second;
		}
		else
		{
			assembly = new (cm_alloc<ScriptAssembly>()) ScriptAssembly();
			mAssemblies[name] = assembly;
		}

		if(!assembly->mIsLoaded)
		{
			assembly->load(path, name);

			// Fully initialize all types that use this assembly
			Vector<ScriptMeta*>::type& mTypeMetas = getTypesToInitialize()[name];
			for(auto& meta : mTypeMetas)
			{
				meta->scriptClass = &assembly->getClass(meta->ns, meta->name);
				meta->thisPtrField = &meta->scriptClass->getField("mCachedPtr");
				meta->initCallback();
			}

			assembly->initialize(entryPoint); // Perform any initialization after everything is loaded
		}

		return *assembly;
	}

	void ScriptManager::unloadAssembly(ScriptAssembly& assembly)
	{
		MonoAssembly* monoAssembly = assembly.mMonoAssembly;
		assembly.unload();

		if(monoAssembly)
			mono_assembly_close(monoAssembly);
	}

	void ScriptManager::registerScriptType(ScriptMeta* metaData)
	{
		Vector<ScriptMeta*>::type& mMetas = getTypesToInitialize()[metaData->assembly];
		mMetas.push_back(metaData);
	}
}