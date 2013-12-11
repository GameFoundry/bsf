#include "BsScriptAssembly.h"
#include "BsScriptClass.h"
#include "CmUtil.h"
#include "CmException.h"

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

	inline bool ScriptAssembly::ClassId::Equals::operator()(const ScriptAssembly::ClassId &a, const ScriptAssembly::ClassId &b) const
	{
		return a.name == b.name && a.namespaceName == b.namespaceName;
	}

	ScriptAssembly::ClassId::ClassId(const String& namespaceName, String name)
		:namespaceName(namespaceName), name(name)
	{

	}

	ScriptAssembly::ScriptAssembly(MonoAssembly* assembly)
	{
		load(assembly);
	}

	ScriptAssembly::~ScriptAssembly()
	{
		for(auto& entry : mClasses)
			cm_delete(entry.second);

		mClasses.clear();
	}

	void ScriptAssembly::load(MonoAssembly* assembly)
	{
		mMonoAssembly = assembly;
		mMonoImage = mono_assembly_get_image(mMonoAssembly);
		if(mMonoImage == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot get script assembly image.");
		}

		mIsLoaded = true;
	}

	void ScriptAssembly::unload()
	{
		if(mMonoImage)
		{
			mono_image_close(mMonoImage);
			mMonoImage = nullptr;
		}

		mIsLoaded = false;
		mMonoAssembly = nullptr;
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

		String fullClassName = namespaceName + "::" + name;
		ScriptClass* newClass = new (cm_alloc<ScriptClass>()) ScriptClass(fullClassName, monoClass);
		mClasses[classId] = newClass;

		return *newClass;
	}
}