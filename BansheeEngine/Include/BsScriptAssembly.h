#pragma once

#include "BsPrerequisites.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace BansheeEngine
{
	class BS_EXPORT ScriptAssembly
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

			ClassId(const CM::String& namespaceName, CM::String name);

			CM::String namespaceName;
			CM::String name;
		};

	public:
		~ScriptAssembly();

		ScriptClass& getClass(const CM::String& namespaceName, const CM::String& name);

	private:
		friend class ScriptManager;

		ScriptAssembly(MonoAssembly* assembly);

		void load(MonoAssembly* assembly);
		void unload();

		MonoImage* mMonoImage;
		MonoAssembly* mMonoAssembly;
		bool mIsLoaded;
		CM::UnorderedMap<ClassId, ScriptClass*, ClassId::Hash, ClassId::Equals>::type mClasses;
	};
}