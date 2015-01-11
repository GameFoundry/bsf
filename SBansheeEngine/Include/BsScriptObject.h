#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptMeta.h"
#include "BsException.h"
#include "BsMonoManager.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	struct ScriptObjectBackup;

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
		virtual bool isPersistent() const { return false; }

		virtual void _clearManagedInstance() { }
		virtual void _restoreManagedInstance() { }
		virtual void _onManagedInstanceDeleted();

		virtual ScriptObjectBackup beginRefresh();
		virtual void endRefresh(const ScriptObjectBackup& data);

	protected:
		MonoObject* mManagedInstance;
	};

	class BS_SCR_BE_EXPORT PersistentScriptObjectBase : public ScriptObjectBase
	{
	public:
		PersistentScriptObjectBase(MonoObject* instance);
		virtual ~PersistentScriptObjectBase();

		virtual bool isPersistent() const { return true; }
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
			// TODO - Use volatile instead?
			initOnStart.makeSureIAmInstantiated();

			Type* param = (Type*)(Base*)this; // Needed due to multiple inheritance. Safe since Type must point to an class derived from this one.

			if(metaData.thisPtrField != nullptr)
				metaData.thisPtrField->setValue(instance, &param);
		}

		virtual ~ScriptObject() 
		{ }

		void _clearManagedInstance()
		{
			if (metaData.thisPtrField != nullptr && mManagedInstance != nullptr)
				metaData.thisPtrField->setValue(mManagedInstance, nullptr);

			mManagedInstance = nullptr;
		}

		void _restoreManagedInstance()
		{
			// Do not construct as constructor usually calls initialization code we don't need when restoring.
			// Managed type needs to account for that on a per-type basis.
			mManagedInstance = _createManagedInstance(false);

			Type* param = (Type*)(Base*)this; // Needed due to multiple inheritance. Safe since Type must point to an class derived from this one.

			if (metaData.thisPtrField != nullptr && mManagedInstance != nullptr)
				metaData.thisPtrField->setValue(mManagedInstance, &param);
		}

		virtual MonoObject* _createManagedInstance(bool construct)
		{
			return metaData.scriptClass->createInstance(construct);
		}

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

	struct ScriptObjectBackup
	{
		Any data;
	};

#define SCRIPT_OBJ(assembly, namespace, name)		\
	static String getAssemblyName() { return assembly; }	\
	static String getNamespace() { return namespace; }		\
	static String getTypeName() { return name; }			\
	static void initRuntimeData();
}