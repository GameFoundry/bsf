#pragma once

#include "BsPrerequisites.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_EXPORT ScriptClass
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
		~ScriptClass();

		ScriptMethod& getMethod(const CM::String& name, CM::UINT32 numParams = 0);
		ScriptField& getField(const CM::String& name);
		ScriptProperty& getProperty(const CM::String& name);

		MonoObject* invokeMethod(const CM::String& name, MonoObject* instance = nullptr, void** params = nullptr, CM::UINT32 numParams = 0);
		void addInternalCall(const CM::String& name, const void* method);

		MonoObject* createInstance() const;
	private:
		friend class ScriptAssembly;

		ScriptClass(const CM::String& fullName, MonoClass* monoClass);

		MonoClass* mClass;
		CM::String mFullName;

		CM::UnorderedMap<MethodId, ScriptMethod*, MethodId::Hash, MethodId::Equals>::type mMethods; 
		CM::UnorderedMap<CM::String, ScriptField*>::type mFields; 
		CM::UnorderedMap<CM::String, ScriptProperty*>::type mProperties;
	};
}