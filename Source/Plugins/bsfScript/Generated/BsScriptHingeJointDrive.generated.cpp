#include "BsScriptHingeJointDrive.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptHingeJointDrive::ScriptHingeJointDrive(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptHingeJointDrive::initRuntimeData()
	{ }

	MonoObject*ScriptHingeJointDrive::box(const HingeJointDrive& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	HingeJointDrive ScriptHingeJointDrive::unbox(MonoObject* value)
	{
		return *(HingeJointDrive*)MonoUtil::unbox(value);
	}

}
