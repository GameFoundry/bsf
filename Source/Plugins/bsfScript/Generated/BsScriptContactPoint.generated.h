#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Physics/BsPhysicsCommon.h"
#include "Math/BsVector3.h"

namespace bs
{
	struct __ContactPointInterop
	{
		Vector3 position;
		Vector3 normal;
		float impulse;
		float separation;
	};

	class BS_SCR_BE_EXPORT ScriptContactPoint : public ScriptObject<ScriptContactPoint>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "ContactPoint")

		static MonoObject* box(const __ContactPointInterop& value);
		static __ContactPointInterop unbox(MonoObject* value);
		static ContactPoint fromInterop(const __ContactPointInterop& value);
		static __ContactPointInterop toInterop(const ContactPoint& value);

	private:
		ScriptContactPoint(MonoObject* managedInstance);

	};
}
