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

	/**
	 * @brief	Helper class to initialize all script interop objects
	 *			as soon as the library is loaded.
	 */
	template <class Type, class Base>
	struct InitScriptObjectOnStart
	{
	public:
		InitScriptObjectOnStart()
		{
			ScriptObject<Type, Base>::_initMetaData();
		}
	};

	/**
	 * @brief	Base class for all script interop objects. Interop
	 *			objects form a connection between C++ and CLR classes
	 *			and methods.
	 */
	class BS_SCR_BE_EXPORT ScriptObjectBase
	{
	public:
		ScriptObjectBase(MonoObject* instance);
		virtual ~ScriptObjectBase();

		/**
		 * @brief	Gets the managed object this interop object represents.
		 */
		MonoObject* getManagedInstance() const { return mManagedInstance; }

		/**
		 * @brief	Should the interop object persist through assembly reload.
		 *			If false then the interop object will be destroyed on reload.
		 */
		virtual bool isPersistent() const { return false; }

		/**
		 * @brief	Clears any managed instance references from the interop object.
		 *			Normally called right after the assemblies are unloaded.
		 */
		virtual void _clearManagedInstance() { }

		/**
		 * @brief	Allows persistent objects to restore their managed instances after
		 *			assembly reload.
		 */
		virtual void _restoreManagedInstance() { }

		/**
		 * @brief	Called when the managed instance gets destroyed. Usually
		 *			triggered by the runtime when the finalizer is called.
		 */
		virtual void _onManagedInstanceDeleted();

		/**
		 * @brief	Called before assembly reload starts to give the object a chance to
		 *			back up its data.
		 */
		virtual ScriptObjectBackup beginRefresh();

		/**
		 * @brief	Called after assembly reload starts to give the object a chance
		 *			to restore the data backed up by the previous ::beginRefresh call.
		 */
		virtual void endRefresh(const ScriptObjectBackup& data);

	protected:
		MonoObject* mManagedInstance;
	};

	/**
	 * @brief	Base class for all persistent interop objects. Persistent objects
	 *			persist through assembly reload.
	 */
	class BS_SCR_BE_EXPORT PersistentScriptObjectBase : public ScriptObjectBase
	{
	public:
		PersistentScriptObjectBase(MonoObject* instance);
		virtual ~PersistentScriptObjectBase();

		/**
		 * @copydoc	ScriptObjectBase::isPersistent 
		 */
		virtual bool isPersistent() const override { return true; }
	};

	/**
	 * @brief	Template version of ScriptObjectBase populates the object
	 *			meta-data on library load.
	 */
	template <class Type, class Base = ScriptObjectBase>
	class ScriptObject : public Base
	{
	public:
		ScriptObject(MonoObject* instance)
			:Base(instance)
		{	
			Type* param = (Type*)(Base*)this; // Needed due to multiple inheritance. Safe since Type must point to an class derived from this one.

			if(metaData.thisPtrField != nullptr)
				metaData.thisPtrField->setValue(instance, &param);
		}

		virtual ~ScriptObject() 
		{ }

		/**
		 * @copydoc	ScriptObjectBase::_clearManagedInstance
		 */
		void _clearManagedInstance()
		{
			if (metaData.thisPtrField != nullptr && mManagedInstance != nullptr)
				metaData.thisPtrField->setValue(mManagedInstance, nullptr);

			mManagedInstance = nullptr;
		}

		/**
		 * @copydoc	ScriptObjectBase::_restoreManagedInstance
		 */
		void _restoreManagedInstance()
		{
			mManagedInstance = _createManagedInstance(true);

			Type* param = (Type*)(Base*)this; // Needed due to multiple inheritance. Safe since Type must point to an class derived from this one.

			if (metaData.thisPtrField != nullptr && mManagedInstance != nullptr)
				metaData.thisPtrField->setValue(mManagedInstance, &param);
		}

		/**
		 * @brief	Creates a new managed instance of the type wrapped
		 *			by this interop object.
		 */
		virtual MonoObject* _createManagedInstance(bool construct)
		{
			return metaData.scriptClass->createInstance(construct);
		}

		/**
		 * @brief	Converts a managed instance into a specific interop object.
		 *			Caller must ensure the managed instance is of the proper type.
		 */
		static Type* toNative(MonoObject* managedInstance)
		{
			Type* nativeInstance = nullptr;

			if (metaData.thisPtrField != nullptr && managedInstance != nullptr)
				metaData.thisPtrField->getValue(managedInstance, &nativeInstance);

			return nativeInstance;
		}

		/**
		 * @brief	Returns the meta-data containing class and method information
		 *			for the managed type.
		 */
		static const ScriptMeta* getMetaData() { return &metaData; }

		/**
		 * @brief	Initializes the meta-data containing class and method information
		 *			for the managed type. Called on library load and on assembly reload.
		 */
		static void _initMetaData()
		{
			metaData = ScriptMeta(Type::getAssemblyName(), Type::getNamespace(), Type::getTypeName(), &Type::initRuntimeData);

			MonoManager::registerScriptType(&metaData);
		}

	protected:
		static ScriptMeta metaData;

	private:
		static volatile InitScriptObjectOnStart<Type, Base> initOnStart;
	};

	template <typename Type, typename Base>
	volatile InitScriptObjectOnStart<Type, Base> ScriptObject<Type, Base>::initOnStart;

	template <typename Type, typename Base>
	ScriptMeta ScriptObject<Type, Base>::metaData;

	/**
	 * @brief	Contains backed up interop object data.
	 */
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