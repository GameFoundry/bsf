#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Physics/BsCharacterController.h"
#include "Math/BsVector3.h"

namespace bs
{
	struct __ControllerControllerCollisionInterop
	{
		MonoObject* controller;
		Vector3 position;
		Vector3 normal;
		Vector3 motionDir;
		float motionAmount;
	};

	class BS_SCR_BE_EXPORT ScriptControllerControllerCollision : public ScriptObject<ScriptControllerControllerCollision>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "ControllerControllerCollision")

		static MonoObject* box(const __ControllerControllerCollisionInterop& value);
		static __ControllerControllerCollisionInterop unbox(MonoObject* value);
		static ControllerControllerCollision fromInterop(const __ControllerControllerCollisionInterop& value);
		static __ControllerControllerCollisionInterop toInterop(const ControllerControllerCollision& value);

	private:
		ScriptControllerControllerCollision(MonoObject* managedInstance);

	};
}
