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

		MonoAssembly& loadAssembly(const CM::String& path, const CM::String& name, const CM::String& entryPoint = CM::StringUtil::BLANK);
		void unloadAssembly(MonoAssembly& assembly);

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
	};
}