#pragma once

#include "CmPrerequisites.h"
#include <mono/jit/jit.h>

namespace CamelotFramework
{
	class CM_EXPORT ScriptClass
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

			MethodId(const String& name, UINT32 numParams);

			String name;
			UINT32 numParams;
		};

	public:
		~ScriptClass();

		ScriptMethod& getMethod(const String& name, UINT32 numParams = 0);
		
		MonoObject* invokeMethod(const String& name, ScriptObject* instance = nullptr, void** params = nullptr, UINT32 numParams = 0);
		void addInternalCall(const String& name, const void* method);
	private:
		friend class ScriptAssembly;

		ScriptClass(const String& fullName, MonoClass* monoClass);

		MonoClass* mClass;
		String mFullName;

		UnorderedMap<MethodId, ScriptMethod*, MethodId::Hash, MethodId::Equals>::type mMethods; 
	};
}