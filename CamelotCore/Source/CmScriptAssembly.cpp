#include "CmScriptAssembly.h"
#include "CmScriptClass.h"
#include "CmUtil.h"
#include "CmException.h"

namespace CamelotFramework
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
		:mMonoAssembly(assembly)
	{
		mMonoImage = mono_assembly_get_image(mMonoAssembly);
		if(mMonoImage == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot get script assembly image.");
		}
	}

	ScriptAssembly::~ScriptAssembly()
	{
		for(auto& entry : mClasses)
			cm_delete(entry.second);

		mClasses.clear();
	}

	ScriptClass& ScriptAssembly::getClass(const String& namespaceName, const String& name)
	{
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