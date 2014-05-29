#include "BsMonoManager.h"
#include "BsException.h"
#include "BsScriptMeta.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

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
		if(mDomain != nullptr)
		{
			mono_jit_cleanup(mDomain);
			mDomain = nullptr;
		}

		for (auto& entry : mAssemblies)
		{
			unloadAssembly(*entry.second);
			bs_delete(entry.second);
		}

		mAssemblies.clear();
	}

	MonoAssembly& MonoManager::loadAssembly(const String& path, const String& name)
	{
		MonoAssembly* assembly = nullptr;

		if(mDomain == nullptr)
		{
			mDomain = mono_jit_init (path.c_str());
			if(mDomain == nullptr)
			{
				BS_EXCEPT(InternalErrorException, "Cannot initialize Mono runtime.");
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
			assembly->load(path, name);

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

			MonoImage* existingImage = mono_image_loaded("mscorlib");
			if(existingImage != nullptr)
			{
				MonoAssembly* mscorlib = new (bs_alloc<MonoAssembly>()) MonoAssembly();
				mAssemblies["mscorlib"] = mscorlib;

				mscorlib->loadAsDependency(existingImage, "mscorlib");
			}
			else
				loadAssembly("mscorlib", "mscorlib");			
		}

		return *assembly;
	}

	void MonoManager::unloadAssembly(MonoAssembly& assembly)
	{
		::MonoAssembly* monoAssembly = assembly.mMonoAssembly;
		assembly.unload();

		// TODO: Not calling this because it crashed if called before domain was unloaded. Try calling
		// it after. I'm not sure if its even necessary to call it.
		//if(monoAssembly)
			//mono_assembly_close(monoAssembly);
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