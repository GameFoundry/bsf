#include "BsScriptContactPoint.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "Math/BsVector3.h"
#include "Wrappers/BsScriptVector.h"

namespace bs
{
	ScriptContactPoint::ScriptContactPoint(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptContactPoint::initRuntimeData()
	{ }

	MonoObject*ScriptContactPoint::box(const __ContactPointInterop& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	__ContactPointInterop ScriptContactPoint::unbox(MonoObject* value)
	{
		return *(__ContactPointInterop*)MonoUtil::unbox(value);
	}

	ContactPoint ScriptContactPoint::fromInterop(const __ContactPointInterop& value)
	{
		ContactPoint output;
		output.position = value.position;
		output.normal = value.normal;
		output.impulse = value.impulse;
		output.separation = value.separation;

		return output;
	}

	__ContactPointInterop ScriptContactPoint::toInterop(const ContactPoint& value)
	{
		__ContactPointInterop output;
		output.position = value.position;
		output.normal = value.normal;
		output.impulse = value.impulse;
		output.separation = value.separation;

		return output;
	}

}
