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

			MethodId(const String& name, UINT32 numParams);

			String name;
			UINT32 numParams;
		};

	public:
		~MonoClass();

		const String& getNamespace() const { return mNamespace; }
		const String& getTypeName() const { return mTypeName; }
		const String& getFullName() const { return mFullName; }

		MonoMethod& getMethod(const String& name, UINT32 numParams = 0);

		MonoField* getField(const String& name) const;
		MonoProperty& getProperty(const String& name);
		MonoObject* getAttribute(MonoClass* monoClass) const;
		MonoClass* getBaseClass() const;

		/**
		 * @brief	Retrieves a method, expects exact method name with parameters.
		 *
		 * @note	Example: Name = "CreateInstance", Signature = "type,int[]"
		 */
		MonoMethod* getMethodExact(const String& name, const String& signature);

		/**
		 * @brief	Returns all fields belonging to this class.
		 *
		 * @note	Be aware this will not include the fields of any base classes.
		 */
		const Vector<MonoField*> getAllFields() const;

		bool hasAttribute(MonoClass* monoClass) const;
		bool hasField(const String& name) const;
		bool isSubClassOf(const MonoClass* monoClass) const;
		bool isInstanceOfType(MonoObject* object) const;

		MonoObject* invokeMethod(const String& name, MonoObject* instance = nullptr, void** params = nullptr, UINT32 numParams = 0);
		void addInternalCall(const String& name, const void* method);

		::MonoClass* _getInternalClass() const { return mClass; }

		MonoObject* createInstance(bool construct = true) const;
		MonoObject* createInstance(void** params, UINT32 numParams);
		MonoObject* createInstance(const String& ctorSignature, void** params);
	private:
		friend class MonoAssembly;

		MonoClass(const String& ns, const String& type, ::MonoClass* monoClass, const MonoAssembly* parentAssembly);

		const MonoAssembly* mParentAssembly;
		::MonoClass* mClass;
		String mNamespace;
		String mTypeName;
		String mFullName;

		UnorderedMap<MethodId, MonoMethod*, MethodId::Hash, MethodId::Equals> mMethods; 
		mutable UnorderedMap<String, MonoField*> mFields; 
		UnorderedMap<String, MonoProperty*> mProperties;

		mutable bool mHasCachedFields;
		mutable Vector<MonoField*> mCachedFieldList;
	};
}