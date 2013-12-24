#include "BsScriptAssembly.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
#include "CmUtil.h"
#include "CmException.h"

#include <mono/metadata/debug-helpers.h>

using namespace CamelotFramework;

namespace BansheeEngine
{
	inline size_t ScriptAssembly::ClassId::Hash::operator()(const ScriptAssembly::ClassId& v) const
	{
		size_t seed = 0;
		hash_combine(seed, v.namespaceName);
		hash_combine(seed, v.name);
		return seed;
	}

	inline bool ScriptAssembly::ClassId::Equals::operator()(const ScriptAssembly::ClassId& a, const ScriptAssembly::ClassId& b) const
	{
		return a.name == b.name && a.namespaceName == b.namespaceName;
	}

	ScriptAssembly::ClassId::ClassId(const String& namespaceName, String name)
		:namespaceName(namespaceName), name(name)
	{

	}

	ScriptAssembly::ScriptAssembly()
		:mIsLoaded(false), mMonoImage(nullptr), mMonoAssembly(nullptr), mDomain(nullptr)
	{

	}

	ScriptAssembly::~ScriptAssembly()
	{
		unload();

		if(mDomain != nullptr)
		{
			mono_jit_cleanup(mDomain);
			mDomain = nullptr;
		}
	}

	void ScriptAssembly::load(const CM::String& path, const CM::String& name)
	{
		if(mDomain == nullptr)
		{
			mDomain = mono_jit_init (path.c_str());
			if(mDomain == nullptr)
			{
				CM_EXCEPT(InternalErrorException, "Cannot initialize Mono runtime.");
			}
		}

		MonoAssembly* monoAssembly = mono_domain_assembly_open (mDomain, path.c_str());
		if(monoAssembly == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot load Mono assembly: " + path);
		}

		mName = name;
		mMonoAssembly = monoAssembly;
		mMonoImage = mono_assembly_get_image(mMonoAssembly);
		if(mMonoImage == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot get script assembly image.");
		}

		mIsLoaded = true;
	}

	void ScriptAssembly::unload()
	{
		if(!mIsLoaded)
			return;

		for(auto& entry : mClasses)
			cm_delete(entry.second);

		mClasses.clear();

		if(mMonoImage != nullptr)
		{
			mono_image_close(mMonoImage);
			mMonoImage = nullptr;
		}

		mIsLoaded = false;
		mMonoAssembly = nullptr;
	}

	void ScriptAssembly::initialize(const CM::String& entryPoint)
	{
		MonoMethodDesc* methodDesc = mono_method_desc_new(entryPoint.c_str(), false);
		MonoMethod* entry = mono_method_desc_search_in_image(methodDesc, mMonoImage);

		if(entry != nullptr)
			mono_runtime_invoke(entry, nullptr, nullptr, nullptr);
	}

	ScriptClass& ScriptAssembly::getClass(const String& namespaceName, const String& name)
	{
		if(!mIsLoaded)
			CM_EXCEPT(InvalidStateException, "Trying to use an unloaded assembly.");

		ClassId classId(namespaceName, name);
		auto iterFind = mClasses.find(classId);

		if(iterFind != mClasses.end())
			return *iterFind->second;

		MonoClass* monoClass = mono_class_from_name(mMonoImage, namespaceName.c_str(), name.c_str());
		if(monoClass == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot get Mono class: " + namespaceName + "." + name);
		}

		String fullClassName = namespaceName + "." + name;
		ScriptClass* newClass = new (cm_alloc<ScriptClass>()) ScriptClass(fullClassName, monoClass, this);
		mClasses[classId] = newClass;

		return *newClass;
	}
}