#include "BsMonoManager.h"
#include "BsException.h"
#include "BsScriptMeta.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-gc.h>

namespace BansheeEngine
{
	const String MonoManager::MONO_LIB_DIR = "..\\..\\Mono\\lib";
	const String MonoManager::MONO_ETC_DIR = "..\\..\\Mono\\etc";

	MonoManager::MonoManager()
		:mRootDomain(nullptr), mScriptDomain(nullptr), mIsCoreLoaded(false)
	{
		mono_set_dirs(MONO_LIB_DIR.c_str(), MONO_ETC_DIR.c_str()); 
		mono_config_parse(nullptr);

		mRootDomain = mono_jit_init_version("BansheeMono", "v4.0.30319"); // TODO: Allow user-defined version here?
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

		if (mScriptDomain != nullptr)
		{
			mono_domain_set(mono_get_root_domain(), false);
			mono_domain_finalize(mScriptDomain, 2000);
			
			MonoObject* exception = nullptr;
			mono_domain_try_unload(mScriptDomain, &exception);

			if (exception != nullptr)
				MonoUtil::throwIfException(exception);

			mScriptDomain = nullptr;
		}

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
			assembly = new (bs_alloc<MonoAssembly>()) MonoAssembly();
			mAssemblies[name] = assembly;
		}
		
		if(!assembly->mIsLoaded)
		{
			assembly->load(mScriptDomain, path, name);

			// Fully initialize all types that use this assembly
			Vector<ScriptMeta*>& mTypeMetas = getTypesToInitialize()[name];
			for(auto& meta : mTypeMetas)
			{
				meta->scriptClass = assembly->getClass(meta->ns, meta->name);
				if(meta->scriptClass == nullptr)
					BS_EXCEPT(InvalidParametersException, "Unable to find class of type: \"" + meta->ns + "::" + meta->name + "\"");

				if(meta->scriptClass->hasField("mCachedPtr"))
					meta->thisPtrField = meta->scriptClass->getField("mCachedPtr");
				else
					meta->thisPtrField = nullptr;

				meta->initCallback();
			}
		}

		if(!mIsCoreLoaded)
		{
			mIsCoreLoaded = true;

			MonoAssembly* corlib = new (bs_alloc<MonoAssembly>()) MonoAssembly();
			mAssemblies["corlib"] = corlib;

			corlib->loadFromImage(mono_get_corlib(), "corlib");		
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

	String MonoManager::getFullTypeName(MonoObject* obj)
	{
		if(obj == nullptr)
			BS_EXCEPT(InvalidParametersException, "Object cannot be null.");

		::MonoClass* monoClass = mono_object_get_class(obj);

		const char* nameSpaceChars = mono_class_get_namespace(monoClass);
		String namespaceStr;
		if(nameSpaceChars != nullptr)
			namespaceStr = nameSpaceChars;

		const char* typeNameChars = mono_class_get_name(monoClass);
		String typeNameStr;
		if(typeNameChars != nullptr)
			typeNameStr = typeNameChars;

		return namespaceStr + "." + typeNameStr;
	}

	String MonoManager::getNamespace(MonoObject* obj)
	{
		if(obj == nullptr)
			BS_EXCEPT(InvalidParametersException, "Object cannot be null.");

		::MonoClass* monoClass = mono_object_get_class(obj);

		const char* nameSpaceChars = mono_class_get_namespace(monoClass);
		String namespaceStr;
		if(nameSpaceChars != nullptr)
			namespaceStr = nameSpaceChars;

		return namespaceStr;
	}

	String MonoManager::getTypeName(MonoObject* obj)
	{
		if(obj == nullptr)
			BS_EXCEPT(InvalidParametersException, "Object cannot be null.");

		::MonoClass* monoClass = mono_object_get_class(obj);

		const char* typeNameChars = mono_class_get_name(monoClass);
		String typeNameStr;
		if(typeNameChars != nullptr)
			typeNameStr = typeNameChars;

		return typeNameStr;
	}
}