#include "BsScriptD6JointDrive.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptD6JointDrive::ScriptD6JointDrive(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptD6JointDrive::initRuntimeData()
	{ }

	MonoObject*ScriptD6JointDrive::box(const D6JointDrive& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	D6JointDrive ScriptD6JointDrive::unbox(MonoObject* value)
	{
		return *(D6JointDrive*)MonoUtil::unbox(value);
	}

}
