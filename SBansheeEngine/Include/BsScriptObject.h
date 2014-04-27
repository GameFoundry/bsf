#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptMeta.h"
#include "CmException.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	template <class Type>
	struct InitScriptObjectOnStart
	{
	public:
		InitScriptObjectOnStart()
		{
			ScriptObject<Type>::_initMetaData();
		}

		void makeSureIAmInstantiated() { }
	};

	class BS_SCR_BE_EXPORT ScriptObjectBase
	{
	public:
		ScriptObjectBase(MonoObject* instance);
		virtual ~ScriptObjectBase();

		static const ScriptMeta* getMetaData() { return &metaData; }

		MonoObject* getManagedInstance() const { return mManagedInstance; }
		virtual void* getNativeRaw() const { return nullptr; }

		virtual void _onManagedInstanceDeleted();

	protected:
		static ScriptMeta metaData;
		MonoObject* mManagedInstance;

		static void registerTypeWithManager();
		static void* getNativeInstance(MonoObject* managedInstance);
	};

	/**
	 * @brief	 Base class for objects that can be extended using Mono scripting
	 */
	template <class Type>
	class ScriptObject : public ScriptObjectBase
	{
	public:
		ScriptObject(MonoObject* instance)
			:ScriptObjectBase(instance)
		{	
			// Compiler will only generate code for stuff that is directly used, including static data members,
			// so we fool it here like we're using the class directly. Otherwise compiler won't generate the code for the member
			// and our type won't get initialized on start (Actual behavior is a bit more random)
			initOnStart.makeSureIAmInstantiated();
		}

		virtual ~ScriptObject() 
		{ }

		static Type* toNative(MonoObject* managedInstance)
		{
			return reinterpret_cast<Type*>(getNativeInstance(managedInstance));
		}

		static void _initMetaData()
		{
			metaData = ScriptMeta(Type::getAssemblyName(), Type::getNamespace(), Type::getTypeName(), &Type::initRuntimeData);

			registerTypeWithManager();
		}

	protected:
		template <class Type2>
		static void throwIfInstancesDontMatch(ScriptObject<Type2>* lhs, void* rhs)
		{
#if CM_DEBUG_MODE
			if((lhs == nullptr && rhs != nullptr) || (rhs == nullptr && lhs != nullptr) || lhs->getNativeRaw() != rhs)
			{
				CM_EXCEPT(InvalidStateException, "Native and script instance do not match. This usually happens when you modify a native object " \
					" that is also being referenced from script code. You should only modify such objects directly from script code.");
			}
#endif
		}

	private:
		static InitScriptObjectOnStart<Type> initOnStart;
	};

	template <typename Type>
	InitScriptObjectOnStart<Type> ScriptObject<Type>::initOnStart;

#define SCRIPT_OBJ(assembly, namespace, name)		\
	static String getAssemblyName() { return assembly; }	\
	static String getNamespace() { return namespace; }		\
	static String getTypeName() { return name; }			\
	static void initRuntimeData();
}