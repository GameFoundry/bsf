#pragma once

#include "BsMonoPrerequisites.h"
#include "BsScriptMeta.h"
#include "CmModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Loads Mono script assemblies and manages script objects.
	 */
	class BS_MONO_EXPORT MonoManager : public Module<MonoManager>
	{
	public:
		MonoManager();
		~MonoManager();

		MonoAssembly& loadAssembly(const String& path, const String& name);
		void unloadAssembly(MonoAssembly& assembly);

		/**
		 * @brief	Searches all loaded assemblies for the specified class.
		 */
		MonoClass* findClass(const String& ns, const String& typeName);

		/**
		 * @brief	Searches all loaded assemblies for the specified class.
		 */
		MonoClass* findClass(::MonoClass* rawMonoClass);

		/**
		 * @brief	Returns the type name of the provided object, with namespace.
		 *
		 * @param 	obj	If non-null, the object to get the type name of.
		 */
		String getFullTypeName(MonoObject* obj);

		/**
		 * @brief	Returns the namespace of the provided object.
		 *
		 * @param 	obj	If non-null, the object to get the namespace of.
		 */
		String getNamespace(MonoObject* obj);

		/**
		 * @brief	Returns the type name of the provided object, without namespace.
		 *
		 * @param 	obj	If non-null, the object to get the type name of.
		 */
		String getTypeName(MonoObject* obj);

		MonoDomain* getDomain() const { return mDomain; }
		MonoAssembly* getAssembly(const String& name) const;

		static void registerScriptType(ScriptMeta* metaData);
	private:
		static const String MONO_LIB_DIR;
		static const String MONO_ETC_DIR;

		static UnorderedMap<String, Vector<ScriptMeta*>::type>::type& getTypesToInitialize()
		{
			static UnorderedMap<String, Vector<ScriptMeta*>::type>::type mTypesToInitialize;
			return mTypesToInitialize;
		}

		UnorderedMap<String, MonoAssembly*>::type mAssemblies;
		MonoDomain* mDomain;
		bool mIsCoreLoaded;
	};
}