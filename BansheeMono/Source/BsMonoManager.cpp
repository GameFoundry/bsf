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
		:mDomain(nullptr), mIsCoreLoaded(false)
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
				meta->scriptClass = assembly->getClass(meta->ns, meta->name);
				if(meta->scriptClass == nullptr)
					CM_EXCEPT(InvalidParametersException, "Unable to find class of type: \"" + meta->ns + "::" + meta->name + "\"");

				if(meta->scriptClass->hasField("mCachedPtr"))
					meta->thisPtrField = &meta->scriptClass->getField("mCachedPtr");
				else
					meta->thisPtrField = nullptr;

				meta->initCallback();
			}

			assembly->initialize(entryPoint); // Perform any initialization after everything is loaded
		}

		if(!mIsCoreLoaded)
		{
			mIsCoreLoaded = true;

			MonoAssembly* mscorlib = new (cm_alloc<MonoAssembly>()) MonoAssembly();
			mAssemblies["mscorlib"] = mscorlib;
			mscorlib->loadAsDependency(assembly->mMonoImage, "mscorlib");

			mIsCoreLoaded = true;
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

	MonoAssembly* MonoManager::getAssembly(const CM::String& name) const
	{
		auto iterFind = mAssemblies.find(name);

		if(iterFind != mAssemblies.end())
			return iterFind->second;

		return nullptr;
	}

	void MonoManager::registerScriptType(ScriptMeta* metaData)
	{
		Vector<ScriptMeta*>::type& mMetas = getTypesToInitialize()[metaData->assembly];
		mMetas.push_back(metaData);
	}

	MonoClass* MonoManager::findClass(const CM::String& ns, const CM::String& typeName)
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

	String MonoManager::getFullTypeName(MonoObject* obj)
	{
		if(obj == nullptr)
			CM_EXCEPT(InvalidParametersException, "Object cannot be null.");

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
			CM_EXCEPT(InvalidParametersException, "Object cannot be null.");

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
			CM_EXCEPT(InvalidParametersException, "Object cannot be null.");

		::MonoClass* monoClass = mono_object_get_class(obj);

		const char* typeNameChars = mono_class_get_name(monoClass);
		String typeNameStr;
		if(typeNameChars != nullptr)
			typeNameStr = typeNameChars;

		return typeNameStr;
	}
}