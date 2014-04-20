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
			ClassId(const String& namespaceName, String name, ::MonoClass* genericInstance = nullptr);

			String namespaceName;
			String name;
			::MonoClass* genericInstance;
		};

	public:
		virtual ~MonoAssembly();

		MonoClass* getClass(const String& namespaceName, const String& name) const;
		MonoClass* getClass(::MonoClass* rawMonoClass) const;
		const Vector<MonoClass*>::type& getAllClasses() const;

		void invoke(const String& functionName);

	private:
		friend class MonoManager;

		MonoAssembly();

		void load(const String& path, const String& name);
		void loadAsDependency(MonoImage* image, const String& name);
		void unload();

		bool isGenericClass(const String& name) const;

		String mName;
		MonoImage* mMonoImage;
		::MonoAssembly* mMonoAssembly;
		bool mIsLoaded;
		bool mIsDependency;
		
		mutable UnorderedMap<ClassId, MonoClass*, ClassId::Hash, ClassId::Equals>::type mClasses;
		mutable UnorderedMap<::MonoClass*, MonoClass*>::type mClassesByRaw;

		mutable bool mHaveCachedClassList;
		mutable Vector<MonoClass*>::type mCachedClassList;
	};
}