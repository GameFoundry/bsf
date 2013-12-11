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
	const String ScriptManager::DOMAIN_NAME = "BansheeEngine";
	const String ScriptManager::MONO_LIB_DIR = "..\\..\\Mono\\lib";
	const String ScriptManager::MONO_ETC_DIR = "..\\..\\Mono\\etc";

	UnorderedMap<String, Vector<ScriptMeta*>::type>::type ScriptManager::mTypesToInitialize;

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
			unloadAssembly(*entry.second);
			cm_delete(entry.second);
		}

		mAssemblies.clear();

		if(mDomain != nullptr)
			mono_jit_cleanup(mDomain);
	}

	ScriptAssembly& ScriptManager::loadAssembly(const String& path, const String& name)
	{
		auto iterFind = mAssemblies.find(name);
		if(iterFind != mAssemblies.end())
		{
			ScriptAssembly* assembly = iterFind->second;
			if(!assembly->mIsLoaded)
			{
				MonoAssembly* monoAssembly = mono_domain_assembly_open (mDomain, path.c_str());
				if(monoAssembly == nullptr)
				{
					CM_EXCEPT(InvalidParametersException, "Cannot load Mono assembly: " + path);
				}

				assembly->load(monoAssembly);
			}

			return *iterFind->second;
		}

		MonoAssembly* monoAssembly = mono_domain_assembly_open (mDomain, path.c_str());
		if(monoAssembly == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot load Mono assembly: " + path);
		}

		ScriptAssembly* assembly = new (cm_alloc<ScriptAssembly>()) ScriptAssembly(monoAssembly);
		mAssemblies[name] = assembly;

		// Fully initialize all types that use this assembly
		Vector<ScriptMeta*>::type& mTypeMetas = mTypesToInitialize[name];
		for(auto& meta : mTypeMetas)
		{
			meta->scriptClass = &assembly->getClass(meta->ns, meta->name);
			meta->thisPtrField = &meta->scriptClass->getField("mCachedPtr");
			meta->initCallback();
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
		Vector<ScriptMeta*>::type& mMetas = mTypesToInitialize[metaData->assembly];
		mMetas.push_back(metaData);
	}
}