#pragma once

#include "CmPrerequisites.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace CamelotFramework
{
	class CM_EXPORT ScriptAssembly
	{
		struct ClassId
		{
			struct Hash
			{
				inline size_t operator()(const ClassId& v) const;
			};

			struct Equals
			{
				inline bool operator()(const ClassId &a, const ClassId &b) const;
			};

			ClassId(const String& namespaceName, String name);

			String namespaceName;
			String name;
		};

	public:
		~ScriptAssembly();

		ScriptClass& getClass(const String& namespaceName, const String& name);

	private:
		friend class ScriptManager;

		ScriptAssembly(MonoAssembly* assembly);

		MonoImage* mMonoImage;
		MonoAssembly* mMonoAssembly;
		UnorderedMap<ClassId, ScriptClass*, ClassId::Hash, ClassId::Equals>::type mClasses;
	};
}