#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Contains information about a single Mono (i.e. managed) class.
	 */
	class BS_MONO_EXPORT MonoClass
	{
		/**
		 * @brief	Used for uniquely identifying a method in a managed class, normally for use
		 *			in containers.
		 */
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

		/**
		 * @brief	Returns the namespace of this class.
		 */
		const String& getNamespace() const { return mNamespace; }

		/**
		 * @brief	Returns the type name of this class.
		 */
		const String& getTypeName() const { return mTypeName; }

		/**
		 * @brief	Returns the full name (Namespace::TypeName) of this class.
		 */
		const String& getFullName() const { return mFullName; }

		/**
		 * @brief	Returns an object referencing a method with the specified name and number of parameters.
		 *			
		 * @note	If the method is overloaded then you should use "getMethodExact". 
		 *			Does not query base class methods.
		 *			Returns null if method cannot be found.
		 */
		MonoMethod* getMethod(const String& name, UINT32 numParams = 0) const;

		/**
		 * @brief	Returns an object referencing a field with the specified name.
		 *			Does not query base class fields.
		 *			Returns null if field cannot be found.
		 */
		MonoField* getField(const String& name) const;

		/**
		 * @brief	Returns an object referencing a property with the specified name.
		 *			Does not query base class properties.
		 *			Throws an exception if the property cannot be found.
		 */
		MonoProperty& getProperty(const String& name);

		/**
		 * @brief	Returns an instance of an attribute of the specified "monoClass" that is part
		 *			of this class. Returns null if this class type does not have that type of attribute.
		 */
		MonoObject* getAttribute(MonoClass* monoClass) const;

		/**
		 * @brief	Returns the base class of this class. Null if this class has no base.
		 */
		MonoClass* getBaseClass() const;

		/**
		 * @brief	Returns an object referencing a method, expects exact method name with parameters.
		 *			
		 * @note	Does not query base class methods.
		 *			Returns null if method cannot be found.
		 *			Example: name = "CreateInstance", signature = "Vector2,int[]"
		 */
		MonoMethod* getMethodExact(const String& name, const String& signature) const;

		/**
		 * @brief	Returns all fields belonging to this class.
		 *
		 * @note	Be aware this will not include the fields of any base classes.
		 */
		const Vector<MonoField*>& getAllFields() const;

		/**
		 * @brief	Returns all methods belonging to this class.
		 *
		 * @note	Be aware this will not include the methods of any base classes.
		 */
		const Vector<MonoMethod*>& getAllMethods() const;

		/**
		 * @brief	Gets all attributes applied to this class.
		 */
		Vector<MonoClass*> getAllAttributes() const;

		/**
		 * @brief	Check if this class has an attribute of the type "monoClass".
		 */
		bool hasAttribute(MonoClass* monoClass) const;

		/**
		 * @brief	Check if this class has a field with the specified name. Does not
		 *			check base classes.
		 */
		bool hasField(const String& name) const;

		/**
		 * @brief	Checks if this class is a sub class of the specified class.
		 */
		bool isSubClassOf(const MonoClass* monoClass) const;

		/**
		 * @brief	Checks is the provided object instance of this class' type.
		 */
		bool isInstanceOfType(MonoObject* object) const;

		/**
		 * @brief	Shortcut for invoking a method on a class. Invokes a method with the provided name
		 *			and number of parameters.
		 *
		 * @param	name		Name of the method to invoke (no parameter list or brackets.
		 * @param	instance	Object instance on invoke the method on. Null if method is static.
		 * @param	params		Array containing pointers to method parameters. Array length must be equal to number of parameters. 
		 *						Can be null if method has no parameters. For value types parameters should be pointers to the values 
		 *						and for reference types they should be pointers to MonoObject.
		 * @param	numParams	Number of parameters the method accepts.
		 *
		 * @note	You cannot use this to call overloaded methods that have the same number of parameters. Use "getMethodExact" and then
		 *			invoke the method from the returned method object.
		 */
		MonoObject* invokeMethod(const String& name, MonoObject* instance = nullptr, void** params = nullptr, UINT32 numParams = 0);

		/**
		 * @brief	Hooks up an internal call that will trigger the provided method callback when the managed method with the
		 *			specified name is called. If name is not valid this will silently fail.
		 */
		void addInternalCall(const String& name, const void* method);

		/**
		 * @brief	Returns the internal mono representation of the class.
		 */
		::MonoClass* _getInternalClass() const { return mClass; }

		/**
		 * @brief	Creates a new instance of this class and optionally constructs it.
		 *			If you don't construct the instance then you should invoke the ".ctor" method manually
		 *			afterwards.
		 */
		MonoObject* createInstance(bool construct = true) const;

		/**
		 * @brief	Creates a new instance of this class and then constructs it using the constructor
		 *			with the specified number of parameters.
		 *
		 * @param	params		Array containing pointers to constructor parameters. Array length must be equal to number of parameters. 
		 * @param	numParams	Number of parameters the constructor accepts.
		 *
		 * @note	If the class have multiple constructors with the same number of parameters use the other "createInstance" overload
		 *			that allows you to provide exact signature.
		 */
		MonoObject* createInstance(void** params, UINT32 numParams);

		/**
		 * @brief	Creates a new instance of this class and then constructs it using the constructor
		 *			with the specified signature.
		 *
		 * @param	ctorSignature	Method signature. Example: "Vector2,int[]"
		 * @param	params			Array containing pointers to constructor parameters. Array length must be equal to number of parameters. 
		 */
		MonoObject* createInstance(const String& ctorSignature, void** params);
	private:
		friend class MonoAssembly;

		/**
		 * @brief	Constructs a new mono class object.
		 * 
		 * @param	ns				Namespace the class belongs to.
		 * @param	type			Type name of the class.
		 * @param	monoClass		Internal mono class.
		 * @param	parentAssembly	Assembly to which this class belongs.
		 */
		MonoClass(const String& ns, const String& type, ::MonoClass* monoClass, const MonoAssembly* parentAssembly);

		const MonoAssembly* mParentAssembly;
		::MonoClass* mClass;
		String mNamespace;
		String mTypeName;
		String mFullName;

		mutable UnorderedMap<MethodId, MonoMethod*, MethodId::Hash, MethodId::Equals> mMethods; 
		mutable UnorderedMap<String, MonoField*> mFields; 
		UnorderedMap<String, MonoProperty*> mProperties;

		mutable bool mHasCachedFields;
		mutable Vector<MonoField*> mCachedFieldList;

		mutable bool mHasCachedMethods;
		mutable Vector<MonoMethod*> mCachedMethodList;
	};
}