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
	class BS_MONO_EXPORT MonoManager : public CM::Module<MonoManager>
	{
	public:
		MonoManager();
		~MonoManager();

		MonoAssembly& loadAssembly(const CM::String& path, const CM::String& name);
		void unloadAssembly(MonoAssembly& assembly);

		/**
		 * @brief	Searches all loaded assemblies for the specified class.
		 */
		MonoClass* findClass(const CM::String& ns, const CM::String& typeName);

		/**
		 * @brief	Returns the type name of the provided object, with namespace.
		 *
		 * @param 	obj	If non-null, the object to get the type name of.
		 */
		CM::String getFullTypeName(MonoObject* obj);

		/**
		 * @brief	Returns the namespace of the provided object.
		 *
		 * @param 	obj	If non-null, the object to get the namespace of.
		 */
		CM::String getNamespace(MonoObject* obj);

		/**
		 * @brief	Returns the type name of the provided object, without namespace.
		 *
		 * @param 	obj	If non-null, the object to get the type name of.
		 */
		CM::String getTypeName(MonoObject* obj);

		MonoDomain* getDomain() const { return mDomain; }
		MonoAssembly* getAssembly(const CM::String& name) const;

		static void registerScriptType(ScriptMeta* metaData);
	private:
		static const CM::String MONO_LIB_DIR;
		static const CM::String MONO_ETC_DIR;

		static CM::UnorderedMap<CM::String, CM::Vector<ScriptMeta*>::type>::type& getTypesToInitialize()
		{
			static CM::UnorderedMap<CM::String, CM::Vector<ScriptMeta*>::type>::type mTypesToInitialize;
			return mTypesToInitialize;
		}

		CM::UnorderedMap<CM::String, MonoAssembly*>::type mAssemblies;
		MonoDomain* mDomain;
		bool mIsCoreLoaded;
	};
}