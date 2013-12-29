#include "BsMonoManager.h"
#include "CmException.h"
#include "BsScriptMeta.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String MonoManager::MONO_LIB_DIR = "..\\..\\Mono\\lib";
	const String MonoManager::MONO_ETC_DIR = "..\\..\\Mono\\etc";

	MonoManager::MonoManager()
		:mDomain(nullptr)
	{
		mono_set_dirs(MONO_LIB_DIR.c_str(), MONO_ETC_DIR.c_str()); 
		mono_config_parse(nullptr);
	}

	MonoManager::~MonoManager()
	{
		for(auto& entry : mAssemblies)
		{
			unloadAssembly(*entry.second);
			cm_delete(entry.second);
		}

		mAssemblies.clear();

		if(mDomain != nullptr)
		{
			mono_jit_cleanup(mDomain);
			mDomain = nullptr;
		}
	}

	MonoAssembly& MonoManager::loadAssembly(const String& path, const String& name, const String& entryPoint)
	{
		MonoAssembly* assembly = nullptr;

		if(mDomain == nullptr)
		{
			mDomain = mono_jit_init (path.c_str());
			if(mDomain == nullptr)
			{
				CM_EXCEPT(InternalErrorException, "Cannot initialize Mono runtime.");
			}
		}

		auto iterFind = mAssemblies.find(name);
		if(iterFind != mAssemblies.end())
		{
			assembly = iterFind->second;
		}
		else
		{
			assembly = new (cm_alloc<MonoAssembly>()) MonoAssembly();
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

	void MonoManager::unloadAssembly(MonoAssembly& assembly)
	{
		::MonoAssembly* monoAssembly = assembly.mMonoAssembly;
		assembly.unload();

		if(monoAssembly)
			mono_assembly_close(monoAssembly);
	}

	void MonoManager::registerScriptType(ScriptMeta* metaData)
	{
		Vector<ScriptMeta*>::type& mMetas = getTypesToInitialize()[metaData->assembly];
		mMetas.push_back(metaData);
	}
}