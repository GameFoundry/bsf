#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace BansheeEngine
{
	class BS_MONO_EXPORT MonoAssembly
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
		~MonoAssembly();

		MonoClass& getClass(const CM::String& namespaceName, const CM::String& name);
		MonoDomain* getDomain() const { return mDomain; }

	private:
		friend class MonoManager;

		MonoAssembly();

		void load(const CM::String& path, const CM::String& name);
		void unload();

		void initialize(const CM::String& entryPoint);

		CM::String mName;
		MonoDomain* mDomain;
		MonoImage* mMonoImage;
		::MonoAssembly* mMonoAssembly;
		bool mIsLoaded;
		CM::UnorderedMap<ClassId, MonoClass*, ClassId::Hash, ClassId::Equals>::type mClasses;
	};
}