#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Physics/BsCharacterController.h"
#include "Math/BsVector3.h"

namespace bs
{
	struct __ControllerCollisionInterop
	{
		Vector3 position;
		Vector3 normal;
		Vector3 motionDir;
		float motionAmount;
	};

	class BS_SCR_BE_EXPORT ScriptControllerCollision : public ScriptObject<ScriptControllerCollision>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "ControllerCollision")

		static MonoObject* box(const __ControllerCollisionInterop& value);
		static __ControllerCollisionInterop unbox(MonoObject* value);
		static ControllerCollision fromInterop(const __ControllerCollisionInterop& value);
		static __ControllerCollisionInterop toInterop(const ControllerCollision& value);

	private:
		ScriptControllerCollision(MonoObject* managedInstance);

	};
}
