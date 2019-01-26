#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Physics/BsHingeJoint.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptHingeJointDrive : public ScriptObject<ScriptHingeJointDrive>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "HingeJointDrive")

		static MonoObject* box(const HingeJointDrive& value);
		static HingeJointDrive unbox(MonoObject* value);

	private:
		ScriptHingeJointDrive(MonoObject* managedInstance);

	};
}
