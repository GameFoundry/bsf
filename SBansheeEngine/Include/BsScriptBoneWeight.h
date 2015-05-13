#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsMeshData.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptBoneWeight : public ScriptObject <ScriptBoneWeight>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "BoneWeight")

		static BoneWeight unbox(MonoObject* obj);
		static MonoObject* box(const BoneWeight& value);

	private:
		ScriptBoneWeight(MonoObject* instance);
	};
}