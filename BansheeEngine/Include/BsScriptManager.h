#pragma once

#include "BsPrerequisites.h"
#include "BsScriptMeta.h"
#include "CmModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Loads Mono script assemblies and manages script objects.
	 */
	class BS_EXPORT ScriptManager : public CM::Module<ScriptManager>
	{
	public:
		ScriptManager();
		~ScriptManager();

		ScriptAssembly& loadAssembly(const CM::String& path, const CM::String& name);
		void unloadAssembly(ScriptAssembly& assembly);

		MonoDomain* getDomain() const { return mDomain; }

		static void registerScriptType(ScriptMeta* metaData);
	private:
		static const CM::String DOMAIN_NAME;
		static const CM::String MONO_LIB_DIR;
		static const CM::String MONO_ETC_DIR;

		static CM::UnorderedMap<CM::String, CM::Vector<ScriptMeta*>::type>::type& getTypesToInitialize()
		{
			static CM::UnorderedMap<CM::String, CM::Vector<ScriptMeta*>::type>::type mTypesToInitialize;
			return mTypesToInitialize;
		}

		MonoDomain* mDomain;
		CM::UnorderedMap<CM::String, ScriptAssembly*>::type mAssemblies;
	};
}