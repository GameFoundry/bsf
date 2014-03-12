#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace BansheeEngine
{
	class BS_MONO_EXPORT MonoAssembly
	{
	public:
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
		virtual ~MonoAssembly();

		MonoClass* getClass(const CM::String& namespaceName, const CM::String& name) const;
		const CM::Vector<MonoClass*>::type& getAllClasses() const;

	private:
		friend class MonoManager;

		MonoAssembly();

		void load(const CM::String& path, const CM::String& name);
		void loadAsDependency(MonoImage* image, const CM::String& name);
		void unload();

		void initialize(const CM::String& entryPoint);

		CM::String mName;
		MonoImage* mMonoImage;
		::MonoAssembly* mMonoAssembly;
		bool mIsLoaded;
		bool mIsDependency;
		
		mutable CM::UnorderedMap<ClassId, MonoClass*, ClassId::Hash, ClassId::Equals>::type mClasses;

		mutable bool mHaveCachedClassList;
		mutable CM::Vector<MonoClass*>::type mCachedClassList;
	};
}