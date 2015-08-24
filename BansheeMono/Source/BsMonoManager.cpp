#include "BsMonoManager.h"
#include "BsException.h"
#include "BsScriptMeta.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsFileSystem.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/mono-debug.h>

namespace BansheeEngine
{
	const String MONO_LIB_DIR = "..\\..\\Mono\\lib\\";
	const String MONO_ETC_DIR = "..\\..\\Mono\\etc\\";
	const String MONO_COMPILER_DIR = "..\\..\\Mono\\compiler\\";
	const MonoVersion MONO_VERSION = MonoVersion::v4_5;
	
	struct MonoVersionData
	{
		String path;
		String version;
	};

	static const MonoVersionData MONO_VERSION_DATA[2] =
	{
		{ MONO_LIB_DIR + "mono\\4.0", "v4.0.30128" },
		{ MONO_LIB_DIR + "mono\\4.5", "v4.0.30319" }
	};

	MonoManager::MonoManager()
		:mRootDomain(nullptr), mScriptDomain(nullptr), mIsCoreLoaded(false)
	{
		mono_set_dirs(MONO_LIB_DIR.c_str(), MONO_ETC_DIR.c_str());
		mono_set_assemblies_path(MONO_VERSION_DATA[(int)MONO_VERSION].path.c_str());

#if BS_DEBUG_MODE
		mono_set_signal_chaining(true);
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
#endif

		mono_config_parse(nullptr);

		mRootDomain = mono_jit_init_version("BansheeMono", MONO_VERSION_DATA[(int)MONO_VERSION].version.c_str());
		if (mRootDomain == nullptr)
			BS_EXCEPT(InternalErrorException, "Cannot initialize Mono runtime.");
	}

	MonoManager::~MonoManager()
	{
		for (auto& entry : mAssemblies)
		{
			bs_delete(entry.second);
		}

		mAssemblies.clear();

		unloadScriptDomain();

		if (mRootDomain != nullptr)
		{
			mono_jit_cleanup(mRootDomain);
			mRootDomain = nullptr;
		}
	}

	MonoAssembly& MonoManager::loadAssembly(const String& path, const String& name)
	{
		MonoAssembly* assembly = nullptr;

		if (mScriptDomain == nullptr)
		{
			mScriptDomain = mono_domain_create_appdomain(const_cast<char *>(path.c_str()), nullptr);
			mono_domain_set(mScriptDomain, false);

			if (mScriptDomain == nullptr)
			{
				BS_EXCEPT(InternalErrorException, "Cannot create script app domain.");
			}
		}

		auto iterFind = mAssemblies.find(name);
		if(iterFind != mAssemblies.end())
		{
			assembly = iterFind->second;
		}
		else
		{
			assembly = new (bs_alloc<MonoAssembly>()) MonoAssembly(path, name);
			mAssemblies[name] = assembly;
		}
		
		initializeAssembly(*assembly);

		return *assembly;
	}

	void MonoManager::unloadAssembly(MonoAssembly& assembly)
	{
		::MonoAssembly* monoAssembly = assembly.mMonoAssembly;
		assembly.unload();

		if(monoAssembly)
			mono_assembly_close(monoAssembly);
	}

	void MonoManager::initializeAssembly(MonoAssembly& assembly)
	{
		if (!assembly.mIsLoaded)
		{
			assembly.load(mScriptDomain);

			// Fully initialize all types that use this assembly
			Vector<ScriptMeta*>& mTypeMetas = getTypesToInitialize()[assembly.mName];
			for (auto& meta : mTypeMetas)
			{
				meta->scriptClass = assembly.getClass(meta->ns, meta->name);
				if (meta->scriptClass == nullptr)
					BS_EXCEPT(InvalidParametersException, "Unable to find class of type: \"" + meta->ns + "::" + meta->name + "\"");

				if (meta->scriptClass->hasField("mCachedPtr"))
					meta->thisPtrField = meta->scriptClass->getField("mCachedPtr");
				else
					meta->thisPtrField = nullptr;

				meta->initCallback();
			}
		}

		if (!mIsCoreLoaded)
		{
			mIsCoreLoaded = true;

			MonoAssembly* corlib = nullptr;

			auto iterFind = mAssemblies.find("corlib");
			if (iterFind == mAssemblies.end())
			{
				corlib = new (bs_alloc<MonoAssembly>()) MonoAssembly("corlib", "corlib");
				mAssemblies["corlib"] = corlib;
			}
			else
				corlib = iterFind->second;

			corlib->loadFromImage(mono_get_corlib());
		}
	}

	MonoAssembly* MonoManager::getAssembly(const String& name) const
	{
		auto iterFind = mAssemblies.find(name);

		if(iterFind != mAssemblies.end())
			return iterFind->second;

		return nullptr;
	}

	void MonoManager::registerScriptType(ScriptMeta* metaData)
	{
		Vector<ScriptMeta*>& mMetas = getTypesToInitialize()[metaData->assembly];
		mMetas.push_back(metaData);
	}

	MonoClass* MonoManager::findClass(const String& ns, const String& typeName)
	{
		MonoClass* monoClass = nullptr;
		for(auto& assembly : mAssemblies)
		{
			monoClass = assembly.second->getClass(ns, typeName);
			if(monoClass != nullptr)
				return monoClass;
		}

		return nullptr;
	}

	MonoClass* MonoManager::findClass(::MonoClass* rawMonoClass)
	{
		MonoClass* monoClass = nullptr;
		for(auto& assembly : mAssemblies)
		{
			monoClass = assembly.second->getClass(rawMonoClass);
			if(monoClass != nullptr)
				return monoClass;
		}

		return nullptr;
	}

	void MonoManager::unloadScriptDomain()
	{
		if (mScriptDomain != nullptr)
		{
			mono_domain_set(mono_get_root_domain(), false);
			mono_domain_finalize(mScriptDomain, 2000);

			MonoObject* exception = nullptr;
			mono_domain_try_unload(mScriptDomain, &exception);

			if (exception != nullptr)
				MonoUtil::throwIfException(exception);

			mono_gc_collect(mono_gc_max_generation());

			mScriptDomain = nullptr;
		}

		for (auto& assemblyEntry : mAssemblies)
			assemblyEntry.second->unload();

		mAssemblies.clear();
		mIsCoreLoaded = false;
	}

	void MonoManager::loadScriptDomain()
	{
		if (mScriptDomain != nullptr)
			unloadScriptDomain();

		if (mScriptDomain == nullptr)
		{
			mScriptDomain = mono_domain_create_appdomain("ScriptDomain", nullptr);
			mono_domain_set(mScriptDomain, false);

			if (mScriptDomain == nullptr)
			{
				BS_EXCEPT(InternalErrorException, "Cannot create script app domain.");
			}
		}

		if (mScriptDomain != nullptr)
		{
			for (auto& assemblyEntry : mAssemblies)
				initializeAssembly(*assemblyEntry.second);
		}
	}

	Path MonoManager::getFrameworkAssembliesFolder() const
	{
		Path assembliesFolder = FileSystem::getWorkingDirectoryPath();
		assembliesFolder.append(MONO_VERSION_DATA[(int)MONO_VERSION].path);

		return assembliesFolder;
	}

	Path MonoManager::getCompilerPath() const
	{
		Path compilerPath = FileSystem::getWorkingDirectoryPath();
		compilerPath.append(MONO_COMPILER_DIR);

#if BS_PLATFORM == BS_PLATFORM_WIN32
		compilerPath.append("mcs.exe");
#else
		static_assert("Not implemented");
#endif

		return compilerPath;
	}
}