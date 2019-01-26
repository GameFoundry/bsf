#include "BsScriptParticleEmissionMode.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptParticleEmissionMode::ScriptParticleEmissionMode(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptParticleEmissionMode::initRuntimeData()
	{ }

	MonoObject*ScriptParticleEmissionMode::box(const ParticleEmissionMode& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	ParticleEmissionMode ScriptParticleEmissionMode::unbox(MonoObject* value)
	{
		return *(ParticleEmissionMode*)MonoUtil::unbox(value);
	}

}
