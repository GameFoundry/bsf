#pragma once

#include "BsPrerequisites.h"
#include "BsScriptAssembly.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class BS_EXPORT EngineAssembly : public CM::Module<EngineAssembly>
	{
	public:
		EngineAssembly();
		~EngineAssembly();

		ScriptAssembly& getAssembly() const { return *mAssembly; }

	private:
		static const CM::String ENGINE_ASSEMBLY_PATH;
		static const CM::String ENGINE_ASSEMBLY_NAME;
		static const CM::String ASSEMBLY_ENTRY_POINT;

		ScriptAssembly* mAssembly;
	};

	ScriptAssembly& gEngineAssembly();
}