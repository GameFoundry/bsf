#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptSerializableObjectInfo.h"
#include "CmModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT RuntimeScriptObjects : public CM::Module<RuntimeScriptObjects>
	{
	public:
		~RuntimeScriptObjects();

		void refreshScriptObjects(const CM::String& assemblyName);
		bool getSerializableObjectInfo(const CM::String& ns, const CM::String& typeName, std::shared_ptr<ScriptSerializableObjectInfo>& outInfo);
		bool hasSerializableObjectInfo(const CM::String& ns, const CM::String& typeName);

		bool isArray(const MonoClass* monoClass) const;
	private:
		CM::UnorderedMap<CM::String, std::shared_ptr<ScriptSerializableAssemblyInfo>>::type mAssemblyInfos;

		void clearScriptObjects(const CM::String& assemblyName);
	};
}