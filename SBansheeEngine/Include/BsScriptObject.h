#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptMeta.h"
#include "BsException.h"
#include "BsMonoManager.h"
#include "BsMonoField.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	template <class Type, class Base>
	struct InitScriptObjectOnStart
	{
	public:
		InitScriptObjectOnStart()
		{
			ScriptObject<Type, Base>::_initMetaData();
		}

		void makeSureIAmInstantiated() { }
	};

	class BS_SCR_BE_EXPORT ScriptObjectBase
	{
	public:
		ScriptObjectBase(MonoObject* instance);
		virtual ~ScriptObjectBase();

		MonoObject* getManagedInstance() const { return mManagedInstance; }
		virtual void* getNativeRaw() const { return nullptr; }

		virtual void _onManagedInstanceDeleted();

	protected:
		
		MonoObject* mManagedInstance;
	};

	/**
	 * @brief	 Base class for objects that can be extended using Mono scripting
	 */
	template <class Type, class Base = ScriptObjectBase>
	class ScriptObject : public Base
	{
	public:
		ScriptObject(MonoObject* instance)
			:Base(instance)
		{	
			// Compiler will only generate code for stuff that is directly used, including static data members,
			// so we fool it here like we're using the class directly. Otherwise compiler won't generate the code for the member
			// and our type won't get initialized on start (Actual behavior is a bit more random)
			initOnStart.makeSureIAmInstantiated();

			Type* param = (Type*)(Base*)this; // Needed due to multiple inheritance. Safe since Type must point to an class derived from this one.

			if(metaData.thisPtrField != nullptr)
				metaData.thisPtrField->setValue(instance, &param);
		}

		virtual ~ScriptObject() 
		{ }

		static Type* toNative(MonoObject* managedInstance)
		{
			Type* nativeInstance = nullptr;

			if (metaData.thisPtrField != nullptr && managedInstance != nullptr)
				metaData.thisPtrField->getValue(managedInstance, &nativeInstance);

			return nativeInstance;
		}

		static const ScriptMeta* getMetaData() { return &metaData; }

		static void _initMetaData()
		{
			metaData = ScriptMeta(Type::getAssemblyName(), Type::getNamespace(), Type::getTypeName(), &Type::initRuntimeData);

			MonoManager::registerScriptType(&metaData);
		}

	protected:
		static ScriptMeta metaData;

		template <class Type2, class Base2>
		static void throwIfInstancesDontMatch(ScriptObject<Type2, Base2>* lhs, void* rhs)
		{
#if BS_DEBUG_MODE
			if((lhs == nullptr && rhs != nullptr) || (rhs == nullptr && lhs != nullptr) || lhs->getNativeRaw() != rhs)
			{
				BS_EXCEPT(InvalidStateException, "Native and script instance do not match. This usually happens when you modify a native object " \
					" that is also being referenced from script code. You should only modify such objects directly from script code.");
			}
#endif
		}

	private:
		static InitScriptObjectOnStart<Type, Base> initOnStart;
	};

	template <typename Type, typename Base>
	InitScriptObjectOnStart<Type, Base> ScriptObject<Type, Base>::initOnStart;

	template <typename Type, typename Base>
	ScriptMeta ScriptObject<Type, Base>::metaData;

#define SCRIPT_OBJ(assembly, namespace, name)		\
	static String getAssemblyName() { return assembly; }	\
	static String getNamespace() { return namespace; }		\
	static String getTypeName() { return name; }			\
	static void initRuntimeData();
}