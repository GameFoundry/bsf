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

			ClassId();
			ClassId(const CM::String& namespaceName, CM::String name, ::MonoClass* genericInstance = nullptr);

			CM::String namespaceName;
			CM::String name;
			::MonoClass* genericInstance;
		};

	public:
		virtual ~MonoAssembly();

		MonoClass* getClass(const CM::String& namespaceName, const CM::String& name) const;
		MonoClass* getClass(::MonoClass* rawMonoClass) const;
		const CM::Vector<MonoClass*>::type& getAllClasses() const;

		void invoke(const CM::String& functionName);

	private:
		friend class MonoManager;

		MonoAssembly();

		void load(const CM::String& path, const CM::String& name);
		void loadAsDependency(MonoImage* image, const CM::String& name);
		void unload();

		bool isGenericClass(const CM::String& name) const;

		CM::String mName;
		MonoImage* mMonoImage;
		::MonoAssembly* mMonoAssembly;
		bool mIsLoaded;
		bool mIsDependency;
		
		mutable CM::UnorderedMap<ClassId, MonoClass*, ClassId::Hash, ClassId::Equals>::type mClasses;
		mutable CM::UnorderedMap<::MonoClass*, MonoClass*>::type mClassesByRaw;

		mutable bool mHaveCachedClassList;
		mutable CM::Vector<MonoClass*>::type mCachedClassList;
	};
}