#pragma once

#include "BsMonoPrerequisites.h"
#include "BsScriptMeta.h"
#include "BsModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Available Mono versions
	 */
	enum class MonoVersion
	{
		v4_0,
		v4_5
	};

	/**
	 * @brief	Loads Mono script assemblies and manages script objects.
	 */
	class BS_MONO_EXPORT MonoManager : public Module<MonoManager>
	{
	public:
		MonoManager();
		~MonoManager();

		/**
		 * @brief	Loads a new assembly from the provided path.
		 *
		 * @param	path	Absolute path to the assembly .dll.
		 * @param	name	Unique name for the assembly.
		 */
		MonoAssembly& loadAssembly(const String& path, const String& name);

		/**
		 * @brief	Searches all loaded assemblies for the specified class.
		 */
		MonoClass* findClass(const String& ns, const String& typeName);

		/**
		 * @brief	Searches all loaded assemblies for the specified class.
		 */
		MonoClass* findClass(::MonoClass* rawMonoClass);

		/**
		 * @brief	Returns the current Mono domains.
		 */
		MonoDomain* getDomain() const { return mScriptDomain; }

		/**
		 * @brief	Attempts to find a previously loaded assembly with the specified name.
		 *			Returns null if assembly cannot be found.
		 */
		MonoAssembly* getAssembly(const String& name) const;

		/**
		 * @brief	Unloads the active domain all script assemblies are loaded in
		 *			and destroys any managed objects associated with it.
		 */
		void unloadScriptDomain();

		/**
		 * @brief	Loads a new script domain. If another domain already exists it will be unloaded. This will also
		 *			restore any previously loaded assemblies.
		 */
		void loadScriptDomain();

		/**
		 * @brief	Returns the path of the folder where Mono framework assemblies are located. Path is relative
		 * 			to the application executable.
		 */
		Path getFrameworkAssembliesFolder() const;

		/**
		 * @brief	Returns the path to the Mono /etc folder that is required for initializing Mono. Path is relative
		 * 			to the application executable.
		 */
		Path getMonoEtcFolder() const;

		/**
		 * @brief	Returns the absolute path to the Mono compiler.
		 */
		Path getCompilerPath() const;

		/**
		 * @brief	Registers a new script type. This should be done before any assembly loading is done.
		 *			Upon assembly load these script types will be initialized with necessary information about their
		 *			managed counterparts.
		 */
		static void registerScriptType(ScriptMeta* metaData);

		/**  
		 * @brief	Triggered when the assembly domain and all relevant assemblies are about to be unloaded.
		 */
		Event<void()> onDomainUnload;
	private:
		/**
		 * @brief	Initializes a previous loaded assembly.
		 */
		void initializeAssembly(MonoAssembly& assembly);

		/**
		 * @brief	Returns a list of all types that will be initializes with their assembly gets loaded.
		 */
		static UnorderedMap<String, Vector<ScriptMeta*>>& getScriptMetaData()
		{
			static UnorderedMap<String, Vector<ScriptMeta*>> mTypesToInitialize;
			return mTypesToInitialize;
		}

		UnorderedMap<String, MonoAssembly*> mAssemblies;
		MonoDomain* mScriptDomain;
		MonoDomain* mRootDomain;
		bool mIsCoreLoaded;
	};
}