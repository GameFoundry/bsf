#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_MONO_EXPORT MonoClass
	{
		struct MethodId
		{
			struct Hash
			{
				inline size_t operator()(const MethodId& v) const;
			};
			
			struct Equals
			{
				inline bool operator()(const MethodId &a, const MethodId &b) const;
			};

			MethodId(const CM::String& name, CM::UINT32 numParams);

			CM::String name;
			CM::UINT32 numParams;
		};

	public:
		~MonoClass();

		MonoMethod& getMethod(const CM::String& name, CM::UINT32 numParams = 0);
		MonoField& getField(const CM::String& name);
		MonoProperty& getProperty(const CM::String& name);

		MonoObject* invokeMethod(const CM::String& name, MonoObject* instance = nullptr, void** params = nullptr, CM::UINT32 numParams = 0);
		void addInternalCall(const CM::String& name, const void* method);

		MonoObject* createInstance() const;
	private:
		friend class MonoAssembly;

		MonoClass(const CM::String& fullName, ::MonoClass* monoClass, MonoAssembly* parentAssembly);

		MonoAssembly* mParentAssembly;
		::MonoClass* mClass;
		CM::String mFullName;

		CM::UnorderedMap<MethodId, MonoMethod*, MethodId::Hash, MethodId::Equals>::type mMethods; 
		CM::UnorderedMap<CM::String, MonoField*>::type mFields; 
		CM::UnorderedMap<CM::String, MonoProperty*>::type mProperties;
	};
}