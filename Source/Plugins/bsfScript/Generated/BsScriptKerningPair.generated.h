#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Text/BsFontDesc.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptKerningPair : public ScriptObject<ScriptKerningPair>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "KerningPair")

		static MonoObject* box(const KerningPair& value);
		static KerningPair unbox(MonoObject* value);

	private:
		ScriptKerningPair(MonoObject* managedInstance);

	};
}
