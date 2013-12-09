#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include <mono/jit/jit.h>

namespace CamelotFramework
{
	/**
	 * @brief	Loads Mono script assemblies and manages script objects.
	 */
	class CM_EXPORT ScriptManager : public Module<ScriptManager>
	{
	public:
		ScriptManager();
		~ScriptManager();

		ScriptAssembly& loadAssembly(const String& name);

		MonoDomain* getDomain() const { return mDomain; }

	private:
		static const String DOMAIN_NAME;
		static const String MONO_LIB_DIR;
		static const String MONO_ETC_DIR;

		MonoDomain* mDomain;
		UnorderedMap<String, ScriptAssembly*>::type mAssemblies;
	};
}