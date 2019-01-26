#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Mesh/BsMeshData.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptBoneWeight : public ScriptObject<ScriptBoneWeight>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "BoneWeight")

		static MonoObject* box(const BoneWeight& value);
		static BoneWeight unbox(MonoObject* value);

	private:
		ScriptBoneWeight(MonoObject* managedInstance);

	};
}
