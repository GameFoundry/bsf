#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Physics/BsD6Joint.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptD6JointDrive : public ScriptObject<ScriptD6JointDrive>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "D6JointDrive")

		static MonoObject* box(const D6JointDrive& value);
		static D6JointDrive unbox(MonoObject* value);

	private:
		ScriptD6JointDrive(MonoObject* managedInstance);

	};
}
