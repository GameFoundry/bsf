#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptComponent.h"
#include "BsScriptCCollider.generated.h"

namespace bs
{
	class CMeshCollider;

	class BS_SCR_BE_EXPORT ScriptCMeshCollider : public TScriptComponent<ScriptCMeshCollider, CMeshCollider, ScriptCColliderBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "MeshCollider")

		ScriptCMeshCollider(MonoObject* managedInstance, const GameObjectHandle<CMeshCollider>& value);

	private:
		static void Internal_setMesh(ScriptCMeshCollider* thisPtr, MonoObject* mesh);
		static MonoObject* Internal_getMesh(ScriptCMeshCollider* thisPtr);
	};
}
