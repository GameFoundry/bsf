//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Stores data about managed serializable objects in specified assemblies.
	 */
	class BS_SCR_BE_EXPORT ScriptAssemblyManager : public Module<ScriptAssemblyManager>
	{
	public:
		ScriptAssemblyManager();
		~ScriptAssemblyManager();

		/**
		 * @brief	Loads all information about managed serializable objects in an assembly with the specified name.
		 *			Assembly must be currently loaded. Once the data has been loaded you will be able to call
		 *			::getSerializableObjectInfo and ::hasSerializableObjectInfo to retrieve information about
		 *			those objects. If an assembly already had data loaded it will be rebuilt.
		 */
		void loadAssemblyInfo(const String& assemblyName);

		/**
		 * @brief	Clears any assembly data previously loaded with ::loadAssemblyInfo.
		 */
		void clearAssemblyInfo();

		/**
		 * @brief	Returns managed serializable object info for a specific managed type.
		 *
		 * @param	ns			Namespace of the type.
		 * @param	typeName	Name of the type.
		 * @param	outInfo		Output object containing information about the type if the type was found, unmodified otherwise.
		 *
		 * @return	True if the type was found, false otherwise.
		 */
		bool getSerializableObjectInfo(const String& ns, const String& typeName, std::shared_ptr<ManagedSerializableObjectInfo>& outInfo);

		/**
		 * @brief	Generates or retrieves a type info object for the specified managed class,
		 *			if the class is serializable.
		 */
		ManagedSerializableTypeInfoPtr getTypeInfo(MonoClass* monoClass);

		/**
		 * @brief	Checks if the managed serializable object info for the specified type exists.
		 *
		 * @param	ns			Namespace of the type.
		 * @param	typeName	Name of the type.
		 *
		 * @return	True if the object info was found, false otherwise.
		 */
		bool hasSerializableObjectInfo(const String& ns, const String& typeName);

		/**
		 * @brief	Returns names of all assemblies that currently have managed serializable object
		 *			data loaded.
		 */
		Vector<String> getScriptAssemblies() const;

		/**
		 * @brief	Gets the managed class for System.Array type.
		 */
		MonoClass* getSystemArrayClass() const { return mSystemArrayClass; }

		/**
		 * @brief	Gets the managed class for System.Collections.Generic.List<T> type.
		 */
		MonoClass* getSystemGenericListClass() const { return mSystemGenericListClass; }

		/**
		 * @brief	Gets the managed class for System.Collections.Generic.Dictionary<T,U> type.
		 */
		MonoClass* getSystemGenericDictionaryClass() const { return mSystemGenericDictionaryClass; }

		/**
		 * @brief	Gets the managed class for System.Type type.
		 */
		MonoClass* getSystemTypeClass() const { return mSystemTypeClass; }

		/**
		 * @brief	Gets the managed class for BansheeEngine.Component type.
		 */
		MonoClass* getComponentClass() const { return mComponentClass; }

		/**
		 * @brief	Gets the managed class for BansheeEngine.MissingComponent type.
		 */
		MonoClass* getMissingComponentClass() const { return mMissingComponentClass; }

		/**
		 * @brief	Gets the managed class for BansheeEngine.SceneObject type.
		 */
		MonoClass* getSceneObjectClass() const { return mSceneObjectClass; }
	private:
		/**
		 * @brief	Deletes all stored managed serializable object infos for all assemblies.
		 */
		void clearScriptObjects();

		/**
		 * @brief	Initializes the base managed types. These are the types we expect must
		 *			exist in loaded assemblies as they're used for various common operations.
		 */
		void initializeBaseTypes();

		UnorderedMap<String, std::shared_ptr<ManagedSerializableAssemblyInfo>> mAssemblyInfos;
		bool mBaseTypesInitialized;

		MonoClass* mSystemArrayClass;
		MonoClass* mSystemGenericListClass;
		MonoClass* mSystemGenericDictionaryClass;
		MonoClass* mSystemTypeClass;

		MonoClass* mComponentClass;
		MonoClass* mSceneObjectClass;
		MonoClass* mMissingComponentClass;

		MonoClass* mSerializeObjectAttribute;
		MonoClass* mDontSerializeFieldAttribute;
		MonoClass* mSerializeFieldAttribute;
		MonoClass* mHideInInspectorAttribute;
	};
}