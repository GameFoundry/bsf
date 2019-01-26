#include "BsScriptCFixedJoint.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "../../../../../Foundation/bsfCore/Components/BsCFixedJoint.h"

namespace bs
{
	ScriptCFixedJoint::ScriptCFixedJoint(MonoObject* managedInstance, const GameObjectHandle<CFixedJoint>& value)
		:TScriptComponent(managedInstance, value)
	{
	}

	void ScriptCFixedJoint::initRuntimeData()
	{

	}

}
