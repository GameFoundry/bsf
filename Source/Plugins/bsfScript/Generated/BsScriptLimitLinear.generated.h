#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Physics/BsJoint.h"
#include "../../../../../Foundation/bsfCore/Physics/BsJoint.h"

namespace bs
{
	struct __LimitLinearInterop
	{
		float extent;
		float contactDist;
		float restitution;
		Spring spring;
	};

	class BS_SCR_BE_EXPORT ScriptLimitLinear : public ScriptObject<ScriptLimitLinear>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "LimitLinear")

		static MonoObject* box(const __LimitLinearInterop& value);
		static __LimitLinearInterop unbox(MonoObject* value);
		static LimitLinear fromInterop(const __LimitLinearInterop& value);
		static __LimitLinearInterop toInterop(const LimitLinear& value);

	private:
		ScriptLimitLinear(MonoObject* managedInstance);

	};
}
